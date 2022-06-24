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
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionShowPage;
}
QT_END_NAMESPACE
using namespace NetworkManager;
class KiranSwitchButton;
#include "connection-lists.h"

class ConnectionShowPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionShowPage(QWidget *parent = nullptr);
    ~ConnectionShowPage() override;
    void initUI();
    void initConnect();

    void setConnectionType(ConnectionSettings::ConnectionType connectionType);
    void setDevicePath(const QString& devicePath);
    void setItemWidgetType(ItemWidgetType itemType);
    void setTitle(QString title);
    void setSwitchButtonVisible(bool visible);
    void setCreateButtonVisible(bool visible);

    // 在kf5-NetworkManager-qt中Connection是指具体的网络配置，不是指已经存在的网络
    void showConnectionLists(ConnectionSettings::ConnectionType type);
    void addConnectionToLists(Connection::Ptr ptr, const QString &devicePath);

    // 在kf5-NetworkManager-qt中Network是指存在的网络
    void showWirelessNetworkLists();
    void addWirelessNetworkToLists(WirelessNetwork::Ptr network,
                                   const QString &devicePath);

    void removeConnectionFromLists(const QString &path);
    void removeWirelessNetworkFromLists(const QString &ssid);

    QListWidgetItem *findItemByUuid(const QString &uuid);
    QListWidgetItem *findItemBySsid(const QString &ssid);
    QListWidgetItem *findItemByActivatedPath(const QString &activatedPath);

    void sortItems();
    int count();
    QListWidgetItem *item(int row);
    void itemSimpleStatus(QListWidgetItem *item);



public slots:
    void clearConnectionLists();

    void updateItemActivatedStatus(const QString &activatedPath);
    void connectionStateNotify(ActiveConnection::State state,const QString &activatedConnectionPath);
    void updateItemActivatingStatus(QListWidgetItem *item);
    void updateItemActivatedPath(QListWidgetItem* item, QString activatedPath);

    void handleActiveStateDeactivated(const QString &activatedConnectionPath);
    void handleToggledSwitchButton(bool toggled);
    void handleWirelessEnabledChanged(bool enabled);
signals:
    void requestCreatConnection();

    void requestEditConnection(const QString &uuid, QString activeConnectionPath);
    void
    requestActivateCurrentItemConnection(const QString &connectionPath,
                                         const QString &connectionParameter = "");
    void requestConnectWirelessNetwork(const ConnectionInfo &connectionInfo);
    void deactivatedItemConnection(const QString &connectionPath);
    void connectionUpdated(const QString &path);
private:
    Ui::ConnectionShowPage *ui;
    KiranSwitchButton *m_switchButton;
    ConnectionSettings::ConnectionType m_connectionType;
    QString m_devicePath;
};

#endif  // KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H
