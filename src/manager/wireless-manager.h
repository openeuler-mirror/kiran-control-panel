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

#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessDevice>
#include <QWidget>
#include "manager.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class WirelessManager;
}
QT_END_NAMESPACE

struct ConnectionInfo;

class WirelessManager : public Manager
{
    Q_OBJECT

public:
    explicit WirelessManager(QWidget *parent = nullptr);
    ~WirelessManager() override;
    void initDevice();
    void initUI();
    void initConnection();


public slots:
    void handleRequestConnectWirelessNetwork(const ConnectionInfo &connectionInfo);
    void getWirelessAvailableConnections(const QString &devicePath);
    void activateWirelessConnection(const QString &connectionPath, const QString &devicePath,const QString &accessPointPath);
    void addAndActivateWirelessConnection(const QString &ssid, const QString &devicePath,const QString &accessPointPath);
    void setWirelessSecuritySetting(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting);
    void setWirelessSetting(const WirelessSetting::Ptr &wirelessSetting);

    void handleActiveConnectionAdded(const QString &path) override;
    void handleActiveConnectionRemoved(const QString &path) override;

//    void handleActiveConnectionStateChanged(ActiveConnection::State state, const QString &path);

    void handleStateActivated(const QString &activatedPath) override;
    void handleStateDeactivated(const QString &deactivatedPath) override;

    void handleNotifierConnectionAdded(const QString &path) override;
    void handleNotifierConnectionRemoved(const QString &path) override;

    void handleNetworkDisappeared(const QString &ssid);
    void handleNetworkAppeared(const QString &ssid);

    void handleReturnPreviousPage();
    void refreshConnectionLists() override;

private:
    Ui::WirelessManager *ui;
    QMap<QString, Connection::Ptr> m_wirelssConnectionMap;
    WirelessDevice::Ptr m_currentWirelessDevice;
};

#endif  //KIRAN_CPANEL_NETWORK_WIRELESS_MANAGER_H
