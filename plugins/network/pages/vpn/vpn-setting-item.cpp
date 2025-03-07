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
#include "vpn-setting-item.h"
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Setting>
#include <NetworkManagerQt/Settings>
#include <QMenu>
#include <QMetaMethod>
#include <QProcess>
#include "connection-manager.h"
#include "logging-category.h"

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
VpnSettingItem::VpnSettingItem(Connection::Ptr& conn, QWidget* parent)
    : ConnectionItem(parent),
      m_connection(conn)
{
    init();
}

VpnSettingItem::~VpnSettingItem()
{
}

void VpnSettingItem::init()
{
    connect(this, &ConnectionItem::requestDisconnect, this, &VpnSettingItem::deactiveVpnConnection);
    connect(this, &ConnectionItem::requestEdit, this, &VpnSettingItem::edit);
    connect(this, &ConnectionItem::requestActive, this, &VpnSettingItem::activeVpnConnection);

    connect(CM_INSTANCE, &ConnectionManager::activeConnectionAdded, this, &VpnSettingItem::processConnectionActived);
    connect(CM_INSTANCE, &ConnectionManager::activeConnectionRemoved, this, &VpnSettingItem::processConnectionDeactived);
    connect(m_connection.data(), &Connection::updated, this, &VpnSettingItem::updateInfo);

    updateInfo();

    // 初始化界面ui默认配置
    setTypeIconVisible(false);
    setEditable(true);
    setDisconnectable(false);

    auto moreMenu = new QMenu(this);
    moreMenu->addAction(tr("Remove"), this, SLOT(remove()));
    moreMenu->setStyleSheet("QPushButton::menu-indicator{image:none}");
    setMoreOptions(moreMenu);

    // 初始化连接激活状态
    QString activeConnectionPath = CM_INSTANCE->connectionActivePath(m_uuid);
    if (activeConnectionPath.isEmpty())
    {
        processConnectionDeactived(ConnectionSettings::Vpn, QString(), m_uuid);
    }
    else
    {
        processConnectionActived(ConnectionSettings::Vpn, activeConnectionPath, m_uuid);
    }
}

void VpnSettingItem::updateInfo()
{
    auto settings = m_connection->settings();
    m_path = m_connection->path();
    m_uuid = settings->uuid();
    m_id = settings->id();
    setName(settings->id());
}

void VpnSettingItem::updateStatus(ActiveConnection::State state)
{
    // 更新连接状态标识，更新按钮状态
    bool isActive = (state == ActiveConnection::State::Activated) || (state == ActiveConnection::State::Activating);
    bool isLoading = (state == ActiveConnection::State::Activating);

    ConnectionItem::updateStatus(isActive, isLoading);
    setDisconnectable(isActive);
}

void VpnSettingItem::processConnectionActived(ConnectionType type, const QString& activeConnectionPath, const QString& uuid)
{
    if (type != ConnectionType::Vpn || uuid != m_uuid)
    {
        return;
    }

    auto activeConn = findActiveConnection(activeConnectionPath);
    if (!activeConn)
    {
        KLOG_WARNING(qLcNetwork) << "vpn active connection" << activeConnectionPath << "not found, uuid:" << uuid;
        return;
    }

    m_activeConnection = activeConn;
    connect(m_activeConnection.data(), &ActiveConnection::stateChanged, this, &VpnSettingItem::updateStatus);

    updateStatus(m_activeConnection->state());
}

void VpnSettingItem::processConnectionDeactived(ConnectionType type, const QString& activeConnectionPath, const QString& uuid)
{
    if (type != ConnectionType::Vpn && uuid != m_uuid)
    {
        return;
    }

    if (m_activeConnection)
    {
        disconnect(m_activeConnection.data(), &ActiveConnection::stateChanged, this, &VpnSettingItem::updateStatus);
    }
    m_activeConnection.reset();

    updateStatus(ActiveConnection::State::Deactivated);
}

void VpnSettingItem::deactiveVpnConnection()
{
    auto reply = NetworkManager::deactivateConnection(m_activeConnection->path());
    checkOpeartionResult(OPERTION_DEACTIVATE, m_id, reply);
}

void VpnSettingItem::activeVpnConnection()
{
    if (m_activeConnection)
    {
        // 配置已激活，跳过操作
        return;
    }
    KLOG_INFO(qLcNetwork) << "active vpn connection:" << m_id;

    auto pendingReply = NetworkManager::activateConnection(m_connection->path(), QString(), QString());
    checkOpeartionResult(OPERTION_ACTIVATE, m_id, pendingReply);
}

void VpnSettingItem::remove()
{
    auto pendingReply = m_connection->remove();
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(pendingReply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &VpnSettingItem::processRemoveReplyFinished);
}

void VpnSettingItem::edit()
{
    const auto connUuid = m_connection->uuid();
    QProcess::startDetached("/usr/bin/nm-connection-editor", {"-e", connUuid});
}

void VpnSettingItem::processRemoveReplyFinished(QDBusPendingCallWatcher* pendingCall)
{
    QDBusPendingReply<QString, QByteArray> reply = *pendingCall;
    if (reply.isError())
    {
        KLOG_WARNING(qLcNetwork) << "remove vpn connection" << m_id << "failed," << reply.error().message();
    }
    pendingCall->deleteLater();
}
}  // namespace Network
}  // namespace Kiran