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

#include "wired-tray-widget.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>

WiredTrayWidget::WiredTrayWidget(const QString &devicePath, QWidget *parent) : ConnectionTray(parent)
{
    m_devicePath = devicePath;
    init();
}

WiredTrayWidget::~WiredTrayWidget()
{
}

void WiredTrayWidget::init()
{
    setFixedWidth(240);
    setContentsMargins(0,0,0,0);
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setSpacing(6);
    m_verticalLayout->setContentsMargins(0, 0, 0, 0);
    m_connectionLists = new ConnectionLists(this);
    m_verticalLayout->addWidget(m_connectionLists);

    initUI();
    initConnection();
}

void WiredTrayWidget::initConnection()
{
    connect(m_connectionLists, &ConnectionLists::connectionUpdated, [=](const QString &path)
    {
        m_connectionLists->removeConnectionFromLists(path);
        Connection::Ptr updateConnection = findConnection(path);
        m_connectionLists->addConnectionToLists(updateConnection,"");
    });

    connect(m_connectionLists,&ConnectionLists::trayRequestConnect,this,&WiredTrayWidget::handleRequestActivateConnection);
    connect(m_connectionLists, &ConnectionLists::trayRequestDisconnect, this,&WiredTrayWidget::handleRequestDisconnect);
}

void WiredTrayWidget::initUI()
{
    showWiredConnectionLists();
}

void WiredTrayWidget::showWiredConnectionLists()
{
    m_connectionLists->setDevicePath(m_devicePath);
    m_connectionLists->setItemWidgetType(ITEM_WIDGET_TYPE_TRAY);
    m_connectionLists->showConnectionLists(ConnectionSettings::Wired);
    m_connectionLists->showWiredStatusIcon();
    m_connectionLists->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void WiredTrayWidget::handleRequestActivateConnection(const ConnectionInfo &connectionInfo)
{
    QString devicePath = "";
    QString connectionParameter = "";
    QString connectionPath = connectionInfo.connectionPath;

    // m_devicePath 可以未空，即不指定设备
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
        KLOG_DEBUG() << "reply.reply():" << reply.reply();
        QString activatedPath = reply.value().path();
    }
}

void WiredTrayWidget::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection = findConnection(path);
    if ((connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
        && (!connection->name().isEmpty()))
    {
        m_connectionLists->addConnectionToLists(connection, m_devicePath);
    }
}

void WiredTrayWidget::handleNotifierConnectionRemoved(const QString &path)
{
    m_connectionLists->removeConnectionFromLists(path);
}

// XXX:可以优化
void WiredTrayWidget::handleStateDeactivated(const QString &activatedPath)
{
    m_connectionLists->handleActiveStateDeactivated(activatedPath);
}

void WiredTrayWidget::handleStateActivated(const QString &activatedPath)
{
    KLOG_DEBUG() << "Wired  handleStateActivated";
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedPath);
    QStringList deviceList =  activeConnection->devices();
    if(deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wired))
    {
        m_connectionLists->updateItemActivatedStatus(activatedPath);
        auto item = m_connectionLists->findItemByActivatedPath(activatedPath);
        ConnectionInfo connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
        StatusNotification::ActiveConnectionActivatedNotify(connectionInfo);
        m_connectionLists->update();
    }
}

void WiredTrayWidget::handleActiveConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "handleActiveConnectionAdded path:" << path;
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if(activatedConnection.isNull())
        return ;
    QStringList deviceList = activatedConnection->devices();
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired && deviceList.contains(m_devicePath))
    {
        QString uuid = activatedConnection->uuid();
        QListWidgetItem *activeItem = m_connectionLists->findItemByUuid(uuid);
        m_connectionLists->updateItemActivatedPath(activeItem,path);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredTrayWidget::handleActiveConnectionStateChanged);
//        connect(activatedConnection.data(), &ActiveConnection::stateChanged, &m_statusNotification, &StatusNotification::ActiveConnectionDeactivatedNotify,Qt::DirectConnection);
    }
}

//需要做判断
void WiredTrayWidget::handleStateActivating(const QString &activatedPath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activatedPath);
    if(activatedConnection.isNull())
        return ;
    QStringList deviceList = activatedConnection->devices();
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired && deviceList.contains(m_devicePath))
    {
        auto item = m_connectionLists->findItemByActivatedPath(activatedPath);
        m_connectionLists->updateItemActivatingStatus(item);
    }
}

void WiredTrayWidget::handleActiveConnectionRemoved(const QString &path)
{
    m_connectionLists->handleActiveStateDeactivated(path);
}

void WiredTrayWidget::handleRequestDisconnect(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
}


