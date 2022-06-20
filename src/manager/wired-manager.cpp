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

WiredManager::WiredManager(QWidget *parent) : Manager(parent), ui(new Ui::WiredManager)
{
    ui->setupUi(this);
    initUI();
    getDeviceList(Device::Ethernet);
    initConnection();
}

WiredManager::~WiredManager()
{
    delete ui;
}

void WiredManager::initUI()
{
    ui->connectionShowPage->setTitle(tr("Wired Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
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

    //检测到新设备时，刷新
    connect(notifier(), &Notifier::deviceAdded, [=](const QString &uni) {});

    connect(notifier(), &Notifier::deviceRemoved, [=](const QString &uni) {});
}

void WiredManager::refreshConnectionLists()
{
    KLOG_DEBUG() << "WiredManager::refreshConnectionLists()";
    ui->connectionShowPage->clearConnectionLists();
    ui->connectionShowPage->showConnectionLists(ConnectionSettings::ConnectionType::Wired);
}

void WiredManager::handleRequestActivateConnection(const QString &connectionPath,const QString &connectionParameter)
{
    m_deviceMap;

    QString devicePath = "";
    KLOG_DEBUG() << "devicePath:" << devicePath;


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

//获取到当前激活对象后，开启等待动画，判断完激活状态后停止等待动画
void WiredManager::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired)
    {
        QString uuid = activatedConnection->uuid();
        int row = ui->connectionShowPage->findItemByUuid(uuid);
        ui->connectionShowPage->setCurrentActiveItem(row);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredManager::handleActiveConnectionStateChanged);
        //加载等待动画
        ui->connectionShowPage->connectionItemLoadingAnimation();
    }
}

void WiredManager::handleActiveConnectionRemoved(const QString &path)
{
    KLOG_DEBUG() << "activeConnectionRemoved:" << path;
}

void WiredManager::handleStateActivated(const QString &activatedPath)
{
    ui->connectionShowPage->connectionStateNotify(ActiveConnection::Activated,activatedPath);
    ui->connectionShowPage->updateActivatedConnectionInfo(activatedPath);
    ui->connectionShowPage->update();
}

void WiredManager::handleStateDeactivated(const QString &deactivatedPath)
{
    ui->connectionShowPage->connectionStateNotify(ActiveConnection::Deactivated,deactivatedPath);
    ui->connectionShowPage->clearDeactivatedConnectionInfo(deactivatedPath);
    ui->connectionShowPage->update();
}

void WiredManager::handleReturnPreviousPage()
{
    ui->wiredSettingPage->clearPtr();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

//TODO:获取当前设备路径devicePath
void WiredManager::handleNotifierConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "WiredManager::handleNotifierConnectionAdded";
    Connection::Ptr connection = findConnection(path);
    if (connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
    {
        ui->connectionShowPage->addConnectionToLists(connection,"");
    }
}

//XXX:当connection被移除时，由于连接可能已经被删除，所有并不能通过findConnection(path)找到该连接对象，进而知道连接类型
void WiredManager::handleNotifierConnectionRemoved(const QString &path)
{
//    KLOG_DEBUG() << "WiredManager::handleNotifierConnectionRemoved";
    ui->connectionShowPage->removeConnectionFromLists(path);
}
