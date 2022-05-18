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

#include "wired-tray-page.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include "connection-lists.h"
WiredTrayPage::WiredTrayPage(QWidget *parent) : TrayPage(parent)
{
    init();
}

WiredTrayPage::~WiredTrayPage()
{
}

void WiredTrayPage::init()
{
    m_connectionLists = getConnectionListsPtr();
    getDeviceList(Device::Ethernet);
    m_deviceList.count();
    initUI();
    initConnection();
}

void WiredTrayPage::initConnection()
{
    connect(m_connectionLists.data(), &ConnectionLists::requestActivateCurrentItemConnection,
            this, &WiredTrayPage::handleRequestActivateConnection);

    connect(m_connectionLists.data(), &ConnectionLists::deactivatedItemConnection,
            this, &WiredTrayPage::handleStateDeactivated);

    connect(m_connectionLists.data(),&ConnectionLists::connectionUpdated,[=](const QString &path){
        m_connectionLists->removeConnectionFromLists(path);
        Connection::Ptr updateConnection = findConnection(path);
        m_connectionLists->addConnectionToLists(updateConnection,"");
    });

    connect(m_connectionLists.data(),&ConnectionLists::trayRequestDisconnect,[=](const QString &activatedConnectionPath){
        QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
        reply.waitForFinished();
        if (reply.isError())
            KLOG_INFO() << "Disconnect failed:" << reply.error();
    });
}


void WiredTrayPage::initUI()
{
    setDeviceLabel(tr("Select wired network card"));
    showWiredConnectionLists();
}

void WiredTrayPage::showWiredConnectionLists()
{
    m_connectionLists->showConnectionLists(ConnectionSettings::Wired, ITEM_WIDGET_TYPE_TRAY);
    m_connectionLists->showWiredStatusIcon();
}

void WiredTrayPage::handleRequestActivateConnection(const QString &connectionPath, const QString &connectionParameter)
{
    QString devicePath = "";

    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, devicePath, connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        //TODO:调用activateConnection失败的处理
        //此处处理进入激活流程失败的原因，并不涉及流程中某个具体阶段失败的原因
        KLOG_DEBUG() << "activate connection failed:" << reply.error();
    }
    else
    {
        KLOG_DEBUG() << "reply.reply():" << reply.reply();
        QString activatedPath = reply.value().path();
    }
}

void WiredTrayPage::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection = findConnection(path);
    if (connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
    {
        m_connectionLists->addConnectionToLists(connection,"");
    }
}

void WiredTrayPage::handleNotifierConnectionRemoved(const QString &path)
{
    m_connectionLists->removeConnectionFromLists(path);
}

void WiredTrayPage::handleStateDeactivated(const QString &deactivatedPath)
{
    KLOG_DEBUG() << "handleStateDeactivated" << deactivatedPath;
    m_connectionLists->connectionStateNotify(ActiveConnection::Deactivated);
    m_connectionLists->clearDeactivatedConnectionInfo(deactivatedPath);
    m_connectionLists->update();
}

void WiredTrayPage::handleStateActivated(const QString &activatedPath)
{
    m_connectionLists->connectionStateNotify(ActiveConnection::Activated);
    m_connectionLists->updateActivatedConnectionInfo(activatedPath);
    m_connectionLists->update();
}

void WiredTrayPage::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired)
    {
        QString uuid = activatedConnection->uuid();
        m_connectionLists->findItemByUuid(uuid);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, [=](NetworkManager::ActiveConnection::State state) {
                    handleActiveConnectionStateChanged(state, path);
                });
        //加载等待动画
        m_connectionLists->connectionItemLoadingAnimation();
    }
}

void WiredTrayPage::handleActiveConnectionRemoved(const QString &path)
{
}

