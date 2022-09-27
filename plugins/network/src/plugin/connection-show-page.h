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

#ifndef KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H
#define KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessNetwork>
#include <QListWidgetItem>
#include <QTimer>
#include <QWidget>
#include "connection-list.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionShowPage;
}
QT_END_NAMESPACE

class KiranSwitchButton;

class ConnectionShowPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionShowPage(QWidget *parent = nullptr);
    ~ConnectionShowPage() override;

    void init(NetworkManager::ConnectionSettings::ConnectionType connectionType, const QString &devicePath);
    void initUI();
    void initConnect();

    void initSwitchButton();

    void setTitle(QString title);
    void setSwitchButtonVisible(bool visible);
    void setCreateButtonVisible(bool visible);

    // 在kf5-NetworkManager-qt中Connection是指具体的网络配置，不是指已经存在的网络
    void showConnectionList(NetworkManager::ConnectionSettings::ConnectionType type);
    void addConnection(NetworkManager::Connection::Ptr ptr, const QString &devicePath);

    // 在kf5-NetworkManager-qt中Network是指存在的网络
    void showWirelessNetworkList();
    void addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network,
                            const QString &devicePath);

    void removeConnectionFromList(const QString &path);
    void removeWirelessNetworkFromList(const QString &ssid);

    QWidget *findItemWidgetByUuid(const QString &uuid);
    QWidget *findItemWidgetBySsid(const QString &ssid);
    QWidget *findItemWidgetByActivePath(const QString &activePath);

    void sort();
    int count();

public slots:
    void clearConnectionList();

    void setItemWidgetStatus(const QString &activePath, NetworkManager::ActiveConnection::State state);

    void updateItemWidgetActivePath(QWidget *widget, QString activatedPath);

    void handleActiveStateDeactivated(const QString &activatedConnectionPath);
    void handleToggledSwitchButton(bool toggled);
    void handleWirelessEnabledChanged(bool enabled);
signals:
    void creatConnection();

    void editConnection(const QString &uuid, QString activeConnectionPath);
    void activateSelectedConnection(const QString &connectionPath,
                                    const QString &connectionParameter = "");
    void activateSelectedWirelessNetwork(const NetworkConnectionInfo &connectionInfo);
    void connectionUpdated(const QString &path);

    void sendSsidToWireless(const QString &ssid);

private:
    Ui::ConnectionShowPage *ui;
    KiranSwitchButton *m_switchButton;
    NetworkManager::ConnectionSettings::ConnectionType m_connectionType;
    QString m_devicePath;
    QTimer m_timer;
    bool m_wirlessNetworkEnable;
};

#endif  // KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H
