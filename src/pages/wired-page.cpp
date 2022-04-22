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
#include "wired-page.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QScrollBar>
#include "ui_wired-page.h"
#include <libnm/nm-connection.h>

WiredPage::WiredPage(QWidget *parent) : Page(parent), ui(new Ui::WiredPage)
{
    ui->setupUi(this);
    initUI();
    getDeviceInfo(Device::Ethernet);
    initConnection();
}

WiredPage::~WiredPage()
{
    delete ui;
}

void WiredPage::initUI()
{
    ui->connectionShowPage->setTitle(tr("Wired Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
    ui->connectionShowPage->showConnectionLists(ConnectionSettings::ConnectionType::Wired);
}


void WiredPage::initConnection()
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

    connect(ui->returnButton, &QPushButton::clicked, this, &WiredPage::handleReturnPreviousPage);

    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        ui->wiredSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wired);
        handleReturnPreviousPage();
    });

    connect(ui->wiredSettingPage, &WiredSettingPage::returnPreviousPage, this, &WiredPage::handleReturnPreviousPage);
    connect(ui->wiredSettingPage, &WiredSettingPage::settingUpdated, [=]() {
        KLOG_DEBUG() << "WiredSettingPage::settingUpdated";
        handleReturnPreviousPage();
        refreshConnectionLists();
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestActivateCurrentItemConnection,
            this, &WiredPage::handleRequestActivateConnection);

    connect(ui->connectionShowPage, &ConnectionShowPage::deactivatedItemConnection,
            this, &WiredPage::handleStateDeactivated);

    initNotifierConnection();

    //检测到新设备时，刷新
    connect(notifier(), &Notifier::deviceAdded, [=](const QString &uni) {
        getDeviceInfo(Device::Ethernet);
    });

    connect(notifier(), &Notifier::deviceRemoved, [=](const QString &uni) {
        getDeviceInfo(Device::Ethernet);
    });
}

void WiredPage::refreshConnectionLists()
{
    KLOG_DEBUG() << "WiredPage::refreshConnectionLists()";
    ui->connectionShowPage->clearConnectionLists();
    ui->connectionShowPage->showConnectionLists(ConnectionSettings::ConnectionType::Wired);
}

void WiredPage::handleRequestActivateConnection(QString connectionPath)
{
    Connection::Ptr connection = findConnection(connectionPath);
    ConnectionSettings::Ptr settings = connection->settings();
    WiredSetting::Ptr wiredSetting = settings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();

    //根据connectionSettings中设置的设备MAC地址，查找对应的设备路径，以便激活连接
    QString macAddress = wiredSetting->macAddress().toHex(':').toUpper();
    QString devicePath = m_deviceMap.value(macAddress);
    KLOG_DEBUG() << "devicePath:" << devicePath;

    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, devicePath, "");

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
void WiredPage::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired)
    {
        QString uuid = activatedConnection->uuid();
        ui->connectionShowPage->findItemByUuid(uuid);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, [=](NetworkManager::ActiveConnection::State state) {
            handleActiveConnectionStateChanged(state, path);
        });
        //加载等待动画
        ui->connectionShowPage->connectionItemLoadingAnimation();
    }
}

void WiredPage::handleActiveConnectionStateChanged(ActiveConnection::State state, const QString &path)
{
    switch (state)
    {
    case ActiveConnection::State::Unknown:
        KLOG_DEBUG() << "ActiveConnection::State::Unknown";
        break;
    case ActiveConnection::State::Activating:
        KLOG_DEBUG() << "ActiveConnection::State::Activating";
        break;
    case ActiveConnection::State::Activated:
        KLOG_DEBUG() << "ActiveConnection::State::Activated";
        handleStateActivated(path);
        break;
    case ActiveConnection::State::Deactivating:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivating";
        break;
    case ActiveConnection::State::Deactivated:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivated";
        handleStateDeactivated(path);
        break;
    default:
        break;
    }
}

void WiredPage::handleActiveConnectionRemoved(const QString &path)
{
    KLOG_DEBUG() << "activeConnectionRemoved:" << path;
}

void WiredPage::handleStateActivated(QString activatedPath)
{
    ui->connectionShowPage->updateActivatedConnectionInfo(activatedPath);
    ui->connectionShowPage->update();
}

void WiredPage::handleStateDeactivated(const QString &deactivatedPath)
{
    KLOG_DEBUG() << "handleStateDeactivated" << deactivatedPath;
    ui->connectionShowPage->clearDeactivatedConnectionInfo(deactivatedPath);
    ui->connectionShowPage->update();
}

void WiredPage::handleReturnPreviousPage()
{
    ui->wiredSettingPage->clearPtr();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void WiredPage::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection = findConnection(path);
    if (connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
    {
        ui->connectionShowPage->addConnectionToLists(connection);
    }
}

void WiredPage::handleNotifierConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->removeConnectionFromLists(path);
}
