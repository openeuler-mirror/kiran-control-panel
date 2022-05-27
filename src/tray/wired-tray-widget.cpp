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
#include "connection-lists.h"
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
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setSpacing(6);
    m_verticalLayout->setContentsMargins(0, 0, 0, 0);
    m_connectionLists = new ConnectionLists(this);
    m_verticalLayout->addWidget(m_connectionLists);

    getDeviceList(Device::Ethernet);
    initUI();
    initConnection();
}

void WiredTrayWidget::initConnection()
{
    connect(m_connectionLists, &ConnectionLists::requestActivateCurrentItemConnection,
            this, &WiredTrayWidget::handleRequestActivateConnection);

    connect(m_connectionLists, &ConnectionLists::connectionUpdated, [=](const QString &path)
    {
        m_connectionLists->removeConnectionFromLists(path);
        Connection::Ptr updateConnection = findConnection(path);
        m_connectionLists->addConnectionToLists(updateConnection,"");
    });

    connect(m_connectionLists.data(), &ConnectionLists::trayRequestDisconnect, [=](const QString &activatedConnectionPath)
    {
        QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
        reply.waitForFinished();
        if (reply.isError())
            KLOG_INFO() << "Disconnect failed:" << reply.error();
    });
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
}

void WiredTrayWidget::handleRequestActivateConnection(const QString &connectionPath, const QString &connectionParameter)
{
    QString devicePath = "";

    // m_devicePath 可以未空，即不指定设备
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, m_devicePath, connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        // TODO:调用activateConnection失败的处理
        // 此处处理进入激活流程失败的原因，并不涉及流程中某个具体阶段失败的原因
        KLOG_ERROR() << "activate connection failed:" << reply.error();
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
    if (connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
    {
        m_connectionLists->addConnectionToLists(connection, "");
    }
}

void WiredTrayWidget::handleNotifierConnectionRemoved(const QString &path)
{
    m_connectionLists->removeConnectionFromLists(path);
}

// XXX:可以优化
void WiredTrayWidget::handleStateDeactivated()
{
    m_connectionLists->handleActiveStateDeactivated();
}

void WiredTrayWidget::handleStateActivated(const QString &activatedPath)
{

    m_connectionLists->connectionStateNotify(ActiveConnection::Activated);
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedPath);
    QStringList deviceList =  activeConnection->devices();
    if(deviceList.contains(m_devicePath))
    {
        m_connectionLists->updateActivatedConnectionInfo(activatedPath);
        m_connectionLists->update();
    }

//    m_connectionLists->connectionStateNotify(ActiveConnection::Activated);
//    m_connectionLists->updateActivatedConnectionInfo(activatedPath);
//    m_connectionLists->update();

}

void WiredTrayWidget::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    QStringList deviceList = activatedConnection->devices();
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired && deviceList.contains(m_devicePath))
    {
        QString uuid = activatedConnection->uuid();
        m_connectionLists->findItemByUuid(uuid);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredTrayWidget::handleActiveConnectionStateChanged);
        // 加载等待动画
        m_connectionLists->connectionItemLoadingAnimation();
    }
}

void WiredTrayWidget::handleActiveConnectionRemoved(const QString &path)
{

}
