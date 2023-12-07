/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#pragma once

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "kiran-collapse/kiran-collapse.h"
#include <QLabel>

class KiranSwitchButton;
class ConnectionItemWidget;

class DeviceAvailableConnectionWidget : public KiranCollapse
{
    Q_OBJECT
public:
    explicit DeviceAvailableConnectionWidget(const QString &devicePath, QWidget *parent = nullptr);
    explicit DeviceAvailableConnectionWidget(NetworkManager::Device::Ptr device, QWidget *parent = nullptr);

    ~DeviceAvailableConnectionWidget();

    QString devicePath() { return m_devicePath; };
    int deviceType() {return m_deviceType;}

private slots:
    void addActiveConnection(const QString &activePath);
    void removeActiveConnection(const QString &activePath);

    void addConnection(const QString &path);
    void removeConnection(const QString &path);
    void createConnection();
    
    void updateConnection(const QString &connectionPath,const QString &mac);

    void disappearNetwork(const QString &ssid);
    void appearNetwork(const QString &ssid);

    void changeDeviceState(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);


    void toggledSwitchButton(bool checked);

private:
    void initUI();
    void initConnect();

    void initWiredAvailableConnections();
    void initWiredDeviceConnect();

    void initWirelessAppearNetwork();
    void initWirelessDeviceConnect();

    void onAddConnection(NetworkManager::Connection::Ptr connection);
    void onAddWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network);

    void addConnectionItem(ConnectionItemWidget *item);
    void removeConnectionItem(ConnectionItemWidget *item);
    void addHiddenNetworkItem();

    void updateConnectionItemStatus(ConnectionItemWidget *item);

    ConnectionItemWidget *findConnectionItemByUuid(const QString &uuid);
    ConnectionItemWidget *findConnectionItemByActivePath(const QString &activePath);
    ConnectionItemWidget *findConnectionItemByPath(const QString &connectionPath);
    ConnectionItemWidget *findConnectionItemBySsid(const QString &ssid);

private:
    QWidget *m_contentWidget;
    QList<ConnectionItemWidget *> m_connectionItemList;
    QVBoxLayout *m_contentWidgetLayout;

    NetworkManager::Device::Ptr m_device;
    NetworkManager::WiredDevice::Ptr m_wiredDevice;
    NetworkManager::WirelessDevice::Ptr m_wirelessDevice;
    QString m_devicePath;
    int m_deviceType;

    KiranSwitchButton *m_switchButton;
    QLabel *m_activateLabel;
    QPushButton *m_createConnectionButton;
};
