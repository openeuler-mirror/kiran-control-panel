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

#ifndef KIRAN_CPANEL_NETWORK_PLUGIN_CONNECTION_LIST_H
#define KIRAN_CPANEL_NETWORK_PLUGIN_CONNECTION_LIST_H

#include "connection-list.h"

class PluginConnectionList : public ConnectionList
{
    Q_OBJECT
public:
    explicit PluginConnectionList(QWidget *parent = nullptr);
    ~PluginConnectionList();

    void initUI();
    void initConnect();

    void addConnection(NetworkManager::Connection::Ptr ptr, const QString &devicePath) override;
    void addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network,
                            const QString &devicePath) override;
    void addHiddenNetworkItemWidget() override;

    void setItemWidgetStatus(const QString &activePath, NetworkManager::ActiveConnection::State state);
    void clearItemWidgetActiveConnectionInfo(QWidget *itemWidget) override;

public slots:
    void handleEditButtonClicked();
    void handleConnectionItemClicked() override;

signals:
    void editConnection(const QString &uuid, QString activeConnectionPath);
    void creatConnection();

    void sendSsidToWireless(const QString &ssid);
};

#endif  // KIRAN_CPANEL_NETWORK_PLUGIN_CONNECTION_LIST_H