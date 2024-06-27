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

#include "wired-tray-widget.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <QLabel>
#include "general.h"
#include "signal-forward.h"
#include "status-notification.h"
#include "utils.h"
using namespace NetworkManager;

WiredTrayWidget::WiredTrayWidget(const QString &devicePath, QWidget *parent)
    : TrayWidget(parent),
      m_devicePath(devicePath)
{
    m_devicePtr = findNetworkInterface(m_devicePath);
    m_wiredDevice = qobject_cast<WiredDevice *>(m_devicePtr);
    init();
}

WiredTrayWidget::~WiredTrayWidget()
{
}

void WiredTrayWidget::init()
{
    initUI();
    initConnection();
    ActiveConnection::Ptr activatedConnection = m_devicePtr->activeConnection();
    if (!activatedConnection.isNull())
    {
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredTrayWidget::handleActiveConnectionStateChanged, Qt::UniqueConnection);
    }
}

// NOTE:设备状态 Device::State::Unavailable和Unmanaged 统一初始化为UnavailableWidget
void WiredTrayWidget::initUI()
{
    // m_wiredDevice->state(); 设备状态在最开始初始化托盘页面时已经判断过了
    KLOG_DEBUG() << "wiredDevice carrier :" << m_wiredDevice->carrier();
    m_connectionList = new TrayConnectionList(this);
    addWidget(m_connectionList);
    showWiredConnectionLists();
}

void WiredTrayWidget::initConnection()
{
    connect(m_connectionList.data(), &TrayConnectionList::connectionUpdated, this, &WiredTrayWidget::handleConnectionUpdated, Qt::UniqueConnection);
    connect(m_connectionList.data(), &TrayConnectionList::activateSelectedConnection, this, &WiredTrayWidget::handleActivateSelectedConnection, Qt::UniqueConnection);
    connect(m_connectionList.data(), &TrayConnectionList::disconnect, this, &WiredTrayWidget::handleDisconnect, Qt::UniqueConnection);
    connect(m_connectionList.data(), &TrayConnectionList::cancelConnection, this, &WiredTrayWidget::handleCancelConnection, Qt::UniqueConnection);
    connect(m_connectionList.data(), &TrayConnectionList::sizeChanged, this, &WiredTrayWidget::sizeChanged, Qt::UniqueConnection);

    connect(m_wiredDevice.data(), &WiredDevice::carrierChanged, this, &WiredTrayWidget::handleCarrierChanged, Qt::UniqueConnection);
    connect(m_wiredDevice.data(), &Device::stateChanged, this, &WiredTrayWidget::handleStateChanged, Qt::UniqueConnection);

    connect(SignalForward::instance(), &SignalForward::wiredConnectionAdded, this, &WiredTrayWidget::handleNotifierConnectionAdded, Qt::UniqueConnection);
    connect(SignalForward::instance(), &SignalForward::connectionRemoved, this, &WiredTrayWidget::handleNotifierConnectionRemoved, Qt::UniqueConnection);
    connect(SignalForward::instance(), &SignalForward::wiredActiveConnectionAdded, this, &WiredTrayWidget::handleActiveConnectionAdded, Qt::UniqueConnection);
    connect(SignalForward::instance(), &SignalForward::activeConnectionRemoved, this, &WiredTrayWidget::handleActiveConnectionRemoved, Qt::UniqueConnection);
}

void WiredTrayWidget::showWiredConnectionLists()
{
    m_connectionList->setDevicePath(m_devicePath);
    m_connectionList->showConnectionList(ConnectionSettings::Wired);
}

// 网线插入后，wiredDevice state 还处在不可用状态,因此无法立即显示出可用连接
/**
 * 暂时不使用CarrierChanged信号处理网线插拔情况，使用更改NetworkManager的配置方式处理。
 * 1、如果不更改配置，网线改变时只有CarrierChanged信号，NetworkManager不会发出其他信号，例如设备状态改变信号
 * 不会断开网络，需要自己处理流程
 * 2、如果更改配置，网线改变时，NetworkManager会自动断开已激活的网络，发出的信号较全。
 */
void WiredTrayWidget::handleCarrierChanged(bool plugged)
{
    KLOG_DEBUG() << "Carrier Changed plugged: " << plugged;
}

void WiredTrayWidget::handleStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    // XXX:此处是对网线插拔的冗余操作，
    // 因为偶现过一次，StateChanged信号丢失Device::Unavailable的情况
    if (!m_wiredDevice.isNull())
    {
        if (m_wiredDevice->carrier())
        {
        }
        else
        {
        }
    }
}

void WiredTrayWidget::handleActivateSelectedConnection(const QString &connectionPath, const QString &connectionParameter)
{
    KLOG_DEBUG() << "Activate Selected Connection:" << connectionPath;
    // m_devicePath 可以为空，即不指定设备
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, m_devicePath, connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        // 此处处理进入激活流程失败的原因，并不涉及流程中某个具体阶段失败的原因
        KLOG_ERROR() << "activate connection failed:" << reply.error();
        StatusNotification::connectitonFailedNotify(connectionPath);
    }
    else
    {
        KLOG_DEBUG() << "reply:" << reply.reply();
    }
}

void WiredTrayWidget::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection = findConnection(path);
    if (NetworkUtils::isAvailableConnection(m_devicePath, connection))
    {
        m_connectionList->addConnection(connection, m_devicePath);
    }
}

void WiredTrayWidget::handleNotifierConnectionRemoved(const QString &path)
{
    if (!m_connectionList.isNull())
    {
        m_connectionList->removeConnectionFromList(path);
        m_connectionList->adjustTraySize();
    }
}

// XXX:可以优化
void WiredTrayWidget::handleStateDeactivated(const QString &activePath)
{
    if (!m_connectionList.isNull())
    {
        m_connectionList->handleActiveStateDeactivated(activePath);
    }
}

void WiredTrayWidget::handleStateActivated(const QString &activePath)
{
    KLOG_DEBUG() << "Wired  handleStateActivated";
    m_connectionList->setItemWidgetStatus(activePath, ActiveConnection::Activated);
    m_connectionList->sort();
}

void WiredTrayWidget::handleActiveConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "handleActiveConnectionAdded path:" << path;
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection.isNull())
    {
        KLOG_DEBUG() << "activatedConnection is null";
        return;
    }

    QStringList deviceList = activatedConnection->devices();
    if (deviceList.contains(m_devicePath))
    {
        QString uuid = activatedConnection->uuid();
        auto *activeItemWidget = m_connectionList->findItemWidgetByUuid(uuid);
        if (activeItemWidget != nullptr)
        {
            m_connectionList->updateItemWidgetActivePath(activeItemWidget, path);
            KLOG_DEBUG() << "activatedConnection->state():" << activatedConnection->state();
            switch (activatedConnection->state())
            {
            case ActiveConnection::State::Activating:
                handleStateActivating(path);
                break;
            case ActiveConnection::State::Activated:
                handleStateActivated(path);
                break;
            default:
                break;
            }
        }
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredTrayWidget::handleActiveConnectionStateChanged, Qt::UniqueConnection);
    }
}

void WiredTrayWidget::handleStateActivating(const QString &activePath)
{
    m_connectionList->setItemWidgetStatus(activePath, ActiveConnection::Activating);
}

void WiredTrayWidget::handleActiveConnectionRemoved(const QString &path)
{
    if (!m_connectionList.isNull())
        m_connectionList->handleActiveStateDeactivated(path);
}

void WiredTrayWidget::handleDisconnect(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
    else
        KLOG_INFO() << "deactivate Connection:" << reply.reply();
}

void WiredTrayWidget::handleCancelConnection(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
    else
        KLOG_INFO() << "deactivate Connection:" << reply.reply();
}

void WiredTrayWidget::handleConnectionUpdated(const QString &path)
{
    m_connectionList->removeConnectionFromList(path);
    Connection::Ptr updateConnection = findConnection(path);
    m_connectionList->addConnection(updateConnection, m_devicePath);
}
