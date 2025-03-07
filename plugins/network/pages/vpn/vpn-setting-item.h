/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include "widgets/connection-item.h"

namespace Kiran
{
namespace Network
{
typedef NetworkManager::ConnectionSettings::ConnectionType ConnectionType;
class VpnSettingItem : public ConnectionItem
{
    Q_OBJECT

public:
    explicit VpnSettingItem(NetworkManager::Connection::Ptr& conn, QWidget* parent = 0);
    ~VpnSettingItem();
    QString path() const { return m_path; };
    QString uuid() const { return m_uuid; };

private:
    void init();
    void updateInfo();

private slots:
    void updateStatus(NetworkManager::ActiveConnection::State state);

    void processConnectionActived(ConnectionType type, const QString& activeConnectionPath, const QString& uuid);
    void processConnectionDeactived(ConnectionType type, const QString& activeConnectionPath, const QString& uuid);

    void activeVpnConnection();
    void deactiveVpnConnection();
    void remove();
    void edit();

    void processRemoveReplyFinished(QDBusPendingCallWatcher* pendingCall);

private:
    NetworkManager::Connection::Ptr m_connection;
    NetworkManager::ActiveConnection::Ptr m_activeConnection;
    QString m_path;
    QString m_uuid;
    QString m_id;
};
}  // namespace Network
}  // namespace Kiran
