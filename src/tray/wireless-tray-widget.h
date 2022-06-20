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

#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/Security8021xSetting>
#include <QWidget>
#include "connection-lists.h"
#include "connection-tray.h"
using namespace NetworkManager;
class WirelessTrayWidget : public ConnectionTray
{
    Q_OBJECT

public:
    explicit WirelessTrayWidget(const QString &devicePath, QWidget *parent = nullptr);
    ~WirelessTrayWidget() override;

    void init();
    void initUI();
    void initConnection();

public slots:
    void handleRequestConnectWirelessNetwork(const ConnectionInfo &connectionInfo);
    void handleRequestConnectHiddenNetwork(const QString &ssid);

    void handleRequestDisconnect(const QString &activatedConnectionPath);
    void handleRequestCancel(const QString &activatedConnectionPath);
    void handleRequestIgnore(const QString &activatedConnectionPath);

    void getWirelessAvailableConnections(const QString &devicePath);
    void activateWirelessConnection(const QString &connectionPath, const QString &devicePath, const QString &accessPointPath);
    void addAndActivateWirelessConnection(ConnectionSettings::Ptr connectionSettings);
    void setSecurityPskAndActivateWirelessConnection(const QString &password);

    void createConnectionSettings(const QString &ssid,const QString &accessPointPath);
    void setWirelessSecurityPsk(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting, const QString &psk);

    void handleActiveConnectionAdded(const QString &path) override;
    void handleActiveConnectionRemoved(const QString &path) override;

    void handleStateActivating(const QString &activatedPath) override;
    void handleStateActivated(const QString &activatedPath) override;
    void handleStateDeactivated(const QString &activatedPath) override;

    void handleNetworkDisappeared(const QString &ssid);
    void handleNetworkAppeared(const QString &ssid);

    void handleDeviceStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason) override;

    void requireInputPassword(const QString &ssid);

private:
    QMap<QString, Connection::Ptr> m_wirelssConnectionMap;
    WirelessDevice::Ptr m_wirelessDevice;
    ConnectionSettings::Ptr m_connectionSettings;
    QString m_devicePath;
    QVBoxLayout *m_verticalLayout;
    QPointer<ConnectionLists> m_connectionLists;
    ConnectionInfo m_connectionInfo;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRELESS_TRAY_WIDGET_H
