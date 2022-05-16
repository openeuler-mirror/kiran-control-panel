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

#ifndef KIRAN_CPANEL_NETWORK_CONNECTION_LISTS_H
#define KIRAN_CPANEL_NETWORK_CONNECTION_LISTS_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessNetwork>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QWidget>
#include "custom-itemwidget.h"

using namespace NetworkManager;

struct WirelessConnectionInfo
{
    int signalStrength;
    QString accessPointPath;
    QString ssid;
    bool securitySetting;
};

struct ConnectionInfo
{
    QString uuid;
    QString connectionPath;
    QString devicePath;
    QString activeConnectionPath;
    bool isWireless = false;
    WirelessConnectionInfo wirelessInfo;
};

Q_DECLARE_METATYPE(ConnectionInfo);
Q_DECLARE_METATYPE(NetworkManager::Status)

class ConnectionLists : public QListWidget
{
    Q_OBJECT

public:
    explicit ConnectionLists(QWidget *parent = nullptr);
    ~ConnectionLists() override;
    void initUI();
    void initConnect();

    // 在kf5-NetworkManager-qt中Connection是指具体的网络配置，不是指已经存在的网络
    void showConnectionLists(ConnectionSettings::ConnectionType type, ItemWidgetType itemType);
    void addConnectionToLists(Connection::Ptr ptr, const QString &devicePath);
    void showWiredStatusIcon();

    // 在kf5-NetworkManager-qt中Network是指存在的网络
    void showWirelessNetworkLists();
    void addWirelessNetworkToLists(WirelessNetwork::Ptr network,
                                   const QString &devicePath);
    void addOtherWirelessItemToLists();

    void removeConnectionFromLists(const QString &path);
    void removeWirelessNetworkFromLists(const QString &ssid);
    void updateItemActivatedPath(QListWidgetItem *item,
                                 QString activatedPath = "");
    void findItemByUuid(const QString &uuid);
    void findItemBySsid(const QString &ssid);

public slots:
    void clearConnectionLists();
    void handleConnectionItemClicked(QListWidgetItem *item);
    void updateActivatedConnectionInfo(QString activatedPath);
    void clearDeactivatedConnectionInfo(const QString &deactivatedPath);
    void connectionStateNotify(ActiveConnection::State state);
    void connectionItemLoadingAnimation();

signals:
    void requestCreatConnection();
    void requestEditConnection(const QString &uuid, QString activeConnectionPath);
    void
    requestActivateCurrentItemConnection(const QString &connectionPath,
                                         const QString &connectionParameter = "");
    void requestConnectWirelessNetwork(const ConnectionInfo &connectionInfo);
    void deactivatedItemConnection(const QString &connectionPath);

private:
    QListWidgetItem *m_previousActivatedItem;
    QListWidgetItem *m_currentActiveItem;
    ItemWidgetType m_itemShowType;
};

class CustomSortListItem : public QListWidgetItem
{
public:
    explicit CustomSortListItem(QWidget *parent = nullptr);
    bool operator<(const QListWidgetItem &other) const;
};

#endif  // KIRAN_CPANEL_NETWORK_CONNECTION_LISTS_H
