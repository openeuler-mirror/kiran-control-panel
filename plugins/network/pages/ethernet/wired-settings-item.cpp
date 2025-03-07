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
#include "wired-settings-item.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <QMenu>
#include <QMouseEvent>
#include <QProcess>
#include "logging-category.h"
#include "page-manager.h"

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
WiredSettingsItem::WiredSettingsItem(WiredDevice::Ptr& device,
                                     Connection::Ptr& connection,
                                     QWidget* parent)
    : ConnectionItem(parent),
      m_device(device),
      m_connection(connection)
{
    init();
}

WiredSettingsItem::~WiredSettingsItem()
{
}

void WiredSettingsItem::init()
{
    m_connectionUUID = m_connection->uuid();
    m_connectionPath = m_connection->path();

    // 监听相关信号，处理配置激活状态变化
    connect(m_device.data(), &Device::activeConnectionChanged, this, &WiredSettingsItem::updateActivatedState);
    connect(m_device.data(), &Device::stateChanged, this, &WiredSettingsItem::updateActivatedState);
    connect(m_device.data(), &Device::connectionStateChanged, this, &WiredSettingsItem::updateActivatedState);
    // 更新连接配置名
    connect(m_connection.data(), &Connection::updated, this, &WiredSettingsItem::updateConnectionName);
    // 断开连接
    connect(this, &ConnectionItem::requestDisconnect, this, &WiredSettingsItem::disconnectConnectionSettings);
    // 编辑连接
    connect(this, &ConnectionItem::requestEdit, this, [this]()
            { emit PM_INSTANCE->requestEditConnectionSettings(m_device->uni(), m_connection->path()); });
    // 激活连接
    connect(this, &ConnectionItem::requestActive, this, &WiredSettingsItem::connectConnectionSettings);

    // 界面默认配置
    setTypeIconVisible(true);
    setTypeIcon("ksvg-kcp-wired-connection");
    setDisconnectable(false);
    setEditable(true);
    setMoreOptionsEnable(true);

    auto moreMenu = new QMenu(this);
    moreMenu->addAction(tr("Remove"), this, SLOT(removeConnectionSettings()));
    moreMenu->addAction(tr("Advanced Settings"), this, SLOT(showAdvancedSettings()));
    moreMenu->setStyleSheet("QPushButton::menu-indicator{image:none}");
    setMoreOptions(moreMenu);

    updateConnectionName();
    updateActivatedState();
}

// 更新连接配置激活状态
void WiredSettingsItem::updateActivatedState()
{
    auto updateUI = [this](const bool isLoading, const bool isActivated)
    {
        ConnectionItem::updateStatus(isActivated, isLoading);
        setDisconnectable(isLoading || isActivated);
    };

    auto isLoading = false;
    auto isActivated = false;

    if (m_device->activeConnection() &&
        m_device->activeConnection()->uuid() == m_connection->uuid())
    {
        /**
         * NOTE:
         * 此处不根据 activeConn->state() 判断，因为activeConn->state()完全连接事件需单独监听ActiveConnection::StateChanged
         * 根据设备的ActivateConnection::uuid判断是否是当前配置被激活，根据设备状态来判断连接当前状态
         */
        isLoading = m_device->state() > Device::State::Disconnected && m_device->state() < Device::State::Activated;
        isActivated = m_device->state() == Device::State::Activated;
    }
    updateUI(isLoading, isActivated);
    KLOG_INFO(qLcNetwork) << "update connection" << m_connection->name() << "activated:" << isActivated << "loading:" << isLoading;
}

void WiredSettingsItem::disconnectConnectionSettings()
{
    auto activeConn = m_device->activeConnection();
    if (activeConn && activeConn->uuid() == m_connection->uuid())
    {
        KLOG_INFO(qLcNetwork) << "disconnect connection:" << m_connection->name() << m_connectionPath;
        auto pendingCall = NetworkManager::deactivateConnection(activeConn->path());
        checkOpeartionResult(OPERTION_DEACTIVATE,m_connectionName,pendingCall);
    }
}

void WiredSettingsItem::connectConnectionSettings()
{
    if (m_device->activeConnection() && m_device->activeConnection()->uuid() == m_connectionUUID)
    {
        return;
    }

    if( m_device->state() <= Device::State::Unavailable )
    {
        dbusNotifyFailed(OPERTION_ACTIVATE,m_connectionName,tr("Device unavailable"));
        return;
    }

    auto pendingReply = NetworkManager::activateConnection(m_connectionPath, m_device->uni(), QString());
    checkOpeartionResult(OPERTION_ACTIVATE,m_connectionName,pendingReply);
}

void WiredSettingsItem::removeConnectionSettings()
{
    QString tip = QString(tr("Are you sure you want to delete the connection %1")).arg(m_connection->name());
    KiranMessageBox::KiranStandardButton btn = KiranMessageBox::message(this, tr("Warning"),
                                                                        tip,
                                                                        KiranMessageBox::Yes | KiranMessageBox::No);
    if (btn != KiranMessageBox::Yes)
    {
        return;
    }

    QString connectionName = m_connection->name();
    QDBusPendingReply<> reply = m_connection->remove();
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_INFO(qLcNetwork) << "Delete the connection failed:" << reply.error();
    }
    else
    {
        KLOG_INFO(qLcNetwork) << "remove connection:" << m_connectionName << m_connectionPath;
    }
}

void WiredSettingsItem::showAdvancedSettings()
{
    QProcess::startDetached("nm-connection-editor", {"-e", m_connectionUUID});
}

void WiredSettingsItem::updateConnectionName()
{
    if (!m_connection)
    {
        return;
    }
    m_connectionName = m_connection->name();
    setName(m_connectionName);
}

}  // namespace Network
}  // namespace Kiran