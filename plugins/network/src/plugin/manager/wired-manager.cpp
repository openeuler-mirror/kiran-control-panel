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
#include "wired-manager.h"
#include <libnm/nm-connection.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QScrollBar>
#include <QPointer>
#include "ui_wired-manager.h"
#include "status-notification.h"

WiredManager::WiredManager(const QString &devicePath, QWidget *parent) : Manager(parent), ui(new Ui::WiredManager)
{
    ui->setupUi(this);
    m_devicePath = devicePath;
    initUI();
    initConnection();
}

WiredManager::~WiredManager()
{
    delete ui;
}

void WiredManager::initUI()
{
    ui->connectionShowPage->setConnectionType(ConnectionSettings::Wired);
    ui->connectionShowPage->setDevicePath(m_devicePath);
    ui->connectionShowPage->setItemWidgetType(ITEM_WIDGET_TYPE_PLUGIN);
    ui->connectionShowPage->setTitle(tr("Wired Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(false);
    ui->connectionShowPage->showConnectionLists(ConnectionSettings::ConnectionType::Wired);
}

void WiredManager::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        ui->wiredSettingPage->showSettingPage();
        QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
        scrollBar->setValue(0);
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestEditConnection, [=](const QString &uuid, QString activeConnectionPath) {
        ui->wiredSettingPage->initConnectionSettings(ConnectionSettings::ConnectionType::Wired, uuid);
        ui->wiredSettingPage->initSettingPage();
        ui->wiredSettingPage->showSettingPage(activeConnectionPath);
        QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
        scrollBar->setValue(0);
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    connect(ui->returnButton, &QPushButton::clicked, this, &WiredManager::handleReturnPreviousPage);

    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        ui->wiredSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wired);
        handleReturnPreviousPage();
    });

    connect(ui->wiredSettingPage, &WiredSettingPage::returnPreviousPage, this, &WiredManager::handleReturnPreviousPage);

    connect(ui->connectionShowPage,&ConnectionShowPage::connectionUpdated,[=](const QString &path){
        KLOG_DEBUG() << "Connection::updated:" << path;
        //移除后再加载进来以更新信息
        ui->connectionShowPage->removeConnectionFromLists(path);
        Connection::Ptr updateConnection = findConnection(path);
        ui->connectionShowPage->addConnectionToLists(updateConnection,"");
        handleReturnPreviousPage();
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestActivateCurrentItemConnection,
            this, &WiredManager::handleRequestActivateConnection);

    connect(ui->connectionShowPage, &ConnectionShowPage::deactivatedItemConnection,
            this, &WiredManager::handleStateDeactivated);

    initNotifierConnection();
}

void WiredManager::handleRequestActivateConnection(const QString &connectionPath,const QString &connectionParameter)
{
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, m_devicePath, connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        //此处处理进入激活流程失败的原因，并不涉及流程中某个具体阶段失败的原因
        KLOG_ERROR() << "activate connection failed:" << reply.error();
        StatusNotification::connectitonFailedNotify(connectionPath);
    }
    else
    {
        KLOG_DEBUG() << "reply.reply():" << reply.reply();
        QString activatedPath = reply.value().path();
    }
}

//获取到当前激活对象后，开启等待动画，判断完激活状态后停止等待动画
void WiredManager::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if(activatedConnection.isNull())
        return ;
    QStringList deviceList = activatedConnection->devices();
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired && deviceList.contains(m_devicePath))
    {
        QString uuid = activatedConnection->uuid();
        QListWidgetItem *activeItem = ui->connectionShowPage->findItemByUuid(uuid);
        ui->connectionShowPage->updateItemActivatedPath(activeItem,path);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredManager::handleActiveConnectionStateChanged);
    }
}

void WiredManager::handleStateActivating(const QString &activatedPath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activatedPath);
    if(activatedConnection.isNull())
        return ;
    QStringList deviceList = activatedConnection->devices();
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired && deviceList.contains(m_devicePath))
    {
        //加载等待动画
        auto item = ui->connectionShowPage->findItemByActivatedPath(activatedPath);
        ui->connectionShowPage->updateItemActivatingStatus(item);
    }
}

void WiredManager::handleActiveConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->handleActiveStateDeactivated(path);
}

//TODO:提升代码，增强复用性
void WiredManager::handleStateActivated(const QString &activatedPath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedPath);
    QStringList deviceList =  activeConnection->devices();
    if(deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wired))
    {
        ui->connectionShowPage->updateItemActivatedStatus(activatedPath);
        ui->connectionShowPage->update();
    }
}

void WiredManager::handleStateDeactivated(const QString &deactivatedPath)
{
    ui->connectionShowPage->handleActiveStateDeactivated(deactivatedPath);
}

void WiredManager::handleReturnPreviousPage()
{
    ui->wiredSettingPage->clearPtr();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}


void WiredManager::handleNotifierConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "WiredManager::handleNotifierConnectionAdded";
    Connection::Ptr connection = findConnection(path);
    if (connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
    {
        ui->connectionShowPage->addConnectionToLists(connection,m_devicePath);
    }
}

//Note:当connection被移除时，由于连接可能已经被删除，所有并不能通过findConnection(path)找到该连接对象，进而知道连接类型
void WiredManager::handleNotifierConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->removeConnectionFromLists(path);
}


