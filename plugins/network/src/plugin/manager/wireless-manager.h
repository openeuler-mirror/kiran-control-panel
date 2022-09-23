/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#ifndef KIRAN_CPANEL_NETWORK_WIRELESS_MANAGER_H
#define KIRAN_CPANEL_NETWORK_WIRELESS_MANAGER_H

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>
#include <QWidget>
#include "connection-list.h"
#include "manager.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class WirelessManager;
}
QT_END_NAMESPACE

class WirelessManager : public Manager
{
    Q_OBJECT

public:
    explicit WirelessManager(const QString &devicePath, QWidget *parent = nullptr);
    ~WirelessManager() override;

    void initUI();
    void initConnection();
    void requireInputPassword(const QString &ssid);

public slots:
    void handleCreatConnection();
    void handleEditConnection(const QString &uuid, QString activeConnectionPath);

    void handleActivateSelectedWirelessNetwork(const NetworkConnectionInfo &connectionInfo);
    void getWirelessAvailableConnections(const QString &devicePath);
    void activateWirelessConnection(const QString &connectionPath, const QString &devicePath, const QString &accessPointPath);
    void addAndActivateWirelessConnection(NetworkManager::ConnectionSettings::Ptr connectionSettings);

    void createConnectionSettings(const QString &ssid, const QString &accessPointPath);
    void setSecurityPskAndActivateWirelessConnection(const QString &password);

    void handleActivateHiddenNetwork(const QString &ssid);

    void handleActiveConnectionAdded(const QString &path) override;
    void handleActiveConnectionRemoved(const QString &path) override;

    void handleStateActivating(const QString &activePath) override;
    void handleStateActivated(const QString &activatedPath) override;
    void handleStateDeactivated(const QString &deactivatedPath) override;

    void handleNotifierConnectionAdded(const QString &path) override;
    void handleNotifierConnectionRemoved(const QString &path) override;

    void handleNetworkDisappeared(const QString &ssid);
    void handleNetworkAppeared(const QString &ssid);

    void handleDeviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason) override;

    void handleReturnPreviousPage();
    void refreshConnectionLists() override;

private:
    Ui::WirelessManager *ui;
    QMap<QString, NetworkManager::Connection::Ptr> m_wirelssConnectionMap;
    NetworkManager::WirelessDevice::Ptr m_wirelessDevice;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;
    NetworkConnectionInfo m_connectionInfo;
    QString m_devicePath;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRELESS_MANAGER_H
