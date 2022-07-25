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

#ifndef KIRAN_CPANEL_NETWORK_WIRELESS_TRAY_WIDGET_H
#define KIRAN_CPANEL_NETWORK_WIRELESS_TRAY_WIDGET_H

#include <NetworkManagerQt/Security8021xSetting>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>
#include <QWidget>
#include "connection-lists.h"
#include "tray-widget.h"

class WirelessTrayWidget : public TrayWidget
{
    Q_OBJECT

public:
    explicit WirelessTrayWidget(const QString &devicePath, QWidget *parent = nullptr);
    ~WirelessTrayWidget() override;

    void init();
    void initUI();
    void initConnection();

public slots:
    void handleRequestConnectWirelessNetwork(const NetworkConnectionInfo &connectionInfo);
    void handleRequestConnectHiddenNetwork(const QString &ssid);

    void handleRequestDisconnect(const QString &activatedConnectionPath);
    void handleRequestCancel(const QString &activatedConnectionPath);
    void handleRequestIgnore(const QString &activatedConnectionPath);

    void getWirelessAvailableConnections(const QString &devicePath);
    void activateWirelessConnection(const QString &connectionPath, const QString &devicePath, const QString &accessPointPath);
    void addAndActivateWirelessConnection(NetworkManager::ConnectionSettings::Ptr connectionSettings);
    void setSecurityPskAndActivateWirelessConnection(const QString &password);

    void createConnectionSettings(const QString &ssid, const QString &accessPointPath);
    void setWirelessSecurityPsk(const NetworkManager::WirelessSecuritySetting::Ptr &wirelessSecuritySetting, const QString &psk);

    void handleActiveConnectionAdded(const QString &path) override;
    void handleActiveConnectionRemoved(const QString &path) override;

    void handleStateActivating(const QString &activatedPath) override;
    void handleStateActivated(const QString &activatedPath) override;
    void handleStateDeactivated(const QString &activatedPath) override;

    void handleNetworkDisappeared(const QString &ssid);
    void handleNetworkAppeared(const QString &ssid);

    void handleDeviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason) override;

    void requireInputPassword(const QString &ssid);

signals:
    void adjustedTraySize(QSize sizeHint);

private:
    QMap<QString, NetworkManager::Connection::Ptr> m_wirelssConnectionMap;
    NetworkManager::WirelessDevice::Ptr m_wirelessDevice;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;
    QString m_devicePath;
    QVBoxLayout *m_verticalLayout;
    QPointer<ConnectionLists> m_connectionLists;
    NetworkConnectionInfo m_connectionInfo;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRELESS_TRAY_WIDGET_H
