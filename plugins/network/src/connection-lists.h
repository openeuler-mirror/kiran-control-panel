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
#include "connection-itemwidget.h"
#include "status-notification.h"

enum ItemWidgetType
{
    ITEM_WIDGET_TYPE_TRAY,
    ITEM_WIDGET_TYPE_PLUGIN
};

struct WirelessConnectionInfo
{
    int signalStrength;
    QString accessPointPath;
    QString ssid;
    bool securitySetting;
};

struct NetworkConnectionInfo
{
    QString id;
    QString uuid;
    QString connectionPath;
    QString devicePath;
    QString activeConnectionPath;
    bool isWireless = false;
    WirelessConnectionInfo wirelessInfo;
};

Q_DECLARE_METATYPE(NetworkConnectionInfo)
Q_DECLARE_METATYPE(NetworkManager::Status)

class ConnectionLists : public QListWidget
{
    Q_OBJECT

public:
    explicit ConnectionLists(QWidget *parent = nullptr);
    ~ConnectionLists() override;
    void initUI();
    void initConnect();

    void setDevicePath(const QString &devicePath);
    void setItemWidgetType(ItemWidgetType itemType);

    // 在kf5-NetworkManager-qt中Connection是指具体的网络配置，不是指已经存在的网络
    void showConnectionLists(NetworkManager::ConnectionSettings::ConnectionType type);
    void addConnectionToLists(NetworkManager::Connection::Ptr ptr, const QString &devicePath);
    void showWiredStatusIcon();

    // 在kf5-NetworkManager-qt中Network是指存在的网络
    void showWirelessNetworkLists();
    void addWirelessNetworkToLists(NetworkManager::WirelessNetwork::Ptr network,
                                   const QString &devicePath);
    void showOtherWirelessItem();

    void removeConnectionFromLists(const QString &path);
    void removeWirelessNetworkFromLists(const QString &ssid);
    void updateItemActivatedPath(QListWidgetItem *item,
                                 QString activatedPath = "");
    // XXX:使用模板进行优化，减少重复代码
    QListWidgetItem *findItemByUuid(const QString &uuid);
    QListWidgetItem *findItemBySsid(const QString &ssid);
    QListWidgetItem *findItemByActivatedPath(const QString &activatedPath);
    QListWidgetItem *getHiddenNetworkItem();
    void enableConnectButtonOfItem(QListWidgetItem *item, bool enable);

    void adjustTraySize();

public slots:
    void handleActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state);

    void handleActiveStateDeactivated(const QString &activatedConnectionPath);
    void handleConnectionUpdated();

    void handleEditButtonClicked();
    void handleDisconnectButtonClicked();
    void handleConnectButtonClicked();
    void handleCancelButtonClicked();
    void handleIgnoreButtonClicked();

    void clearConnectionLists();
    void handleConnectionItemClicked(QListWidgetItem *item);
    void updateItemActivatingStatus(QListWidgetItem *item);
    void updateItemActivatedStatus(const QString &activatedConnectionPath);
    void clearItemActivatedConnectionInfo(QListWidgetItem *activatedItem);

    void handleSendPassword(const QString &password);
    void showInputPasswordWidgetOfItem(QListWidgetItem *item);
    void itemSimpleStatus(QListWidgetItem *item);

protected:
    void hideEvent(QHideEvent *event) override;

signals:
    void requestCreatConnection();
    void requestEditConnection(const QString &uuid, QString activeConnectionPath);
    void requestActivateCurrentItemConnection(const QString &connectionPath,
                                              const QString &connectionParameter = "");
    void requestConnectWirelessNetwork(const NetworkConnectionInfo &connectionInfo);
    void deactivatedItemConnection(const QString &connectionPath);
    void connectionUpdated(const QString &path);

    void trayRequestDisconnect(const QString &activatedConnectionPath);
    void trayRequestConnect(const NetworkConnectionInfo &connectionInfo);
    void trayRequestIgnore(const QString &activatedConnectionPath);
    void trayRequestCancel(const QString &activatedConnectionPath);

    void sendPasswordToWirelessSetting(const QString &password);
    void sendSsidToWireless(const QString &ssid);

    void adjustedTraySize(QSize size);

private:
    ItemWidgetType m_itemShowType;
    QString m_currentDevicePath;
    QMap<QWidget *, QListWidgetItem *> m_itemWidgetMap;
    StatusNotification m_statusNotification;
};

class ConnectionSortListItem : public QListWidgetItem
{
public:
    explicit ConnectionSortListItem(QWidget *parent = nullptr);
    bool operator<(const QListWidgetItem &other) const;
};

#endif  // KIRAN_CPANEL_NETWORK_CONNECTION_LISTS_H
