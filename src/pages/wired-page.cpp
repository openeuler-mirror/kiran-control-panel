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
#include "ui_wired-page.h"

#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QListWidget>

enum WiredEditPages
{
    PAGE_WIRED,
    PAGE_WIRED_SETTING
};

WiredPage::WiredPage(QWidget *parent) : QWidget(parent), ui(new Ui::WiredPage)
{
    ui->setupUi(this);
    initUI();
    initConnecton();

    //XXX:获取wired device接口
    const Device::List deviceList = networkInterfaces();
    KLOG_DEBUG() << "deviceList:" << deviceList;
    for (Device::Ptr dev : deviceList)
    {
        KLOG_DEBUG() << "dev->type():" << dev->type();
        if (dev->type() == Device::Ethernet)
        {
            m_wiredDevice = qobject_cast<WiredDevice *>(dev);
            break;
        }
    }

    if (!m_wiredDevice)
    {
        KLOG_DEBUG() << "Wired device not found";
    }
}

WiredPage::~WiredPage()
{
    delete ui;
}

void WiredPage::initUI()
{
    ui->connectionShowPage->setTitle(tr("Wired Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
    showWiredConnections();
}

void WiredPage::showWiredConnections()
{
    const Connection::List connectionList = NetworkManager::listConnections();
    for (Connection::Ptr conn : connectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
        {
            KLOG_DEBUG() << "________________";
            KLOG_DEBUG() << "conn->uuid():" << conn->uuid();
            KLOG_DEBUG() << "conn->name():" << conn->name();
            KLOG_DEBUG() << "conn->path():" << conn->path();
            KLOG_DEBUG() << "----------------";
            ui->connectionShowPage->addConnectionToLists(conn);
        }
    }
}

void WiredPage::initConnecton()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        ui->wiredSettingPage->refreshSettingPage();
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED_SETTING);
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestEditConnection, [=](QString uuid, QString activeConnectionPath) {
        ui->wiredSettingPage->initConnectionSettings(ConnectionSettings::ConnectionType::Wired, uuid);
        ui->wiredSettingPage->refreshSettingPage(activeConnectionPath);
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED_SETTING);
    });

    connect(ui->wiredSettingPage, &WiredSettingPage::returnPreviousPage, [=]() {
        ui->wiredSettingPage->clearPtr();
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED);
    });

    connect(ui->wiredSettingPage, &WiredSettingPage::settingChanged, [=]() {
        ui->wiredSettingPage->clearPtr();
        ui->connectionShowPage->clearConnectionLists();
        //保存设置生效后，刷新列表时，NetworkManager::listConnections()接口返回的数据还未更新，故延时等待
        //测试等待20msec后能够得到更新的数据
        QTimer::singleShot(30, this, SLOT(showWiredConnections()));
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED);
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::activateCurrentItemConnection, this, &WiredPage::handleActivateConnection);

    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, [=](const QString &path) {
    });

    //    SettingsNotifier::connectionAddComplete()
}

void WiredPage::handleActivateConnection(QString connectionPath)
{
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, m_wiredDevice->uni(), "");

    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_DEBUG() << "activate connection failed" << reply.error();
    }
    else
    {
        KLOG_DEBUG() << "reply.value().path():" << reply.value().path();
        QString activatedPath = reply.value().path();
        ActiveConnection::Ptr activatedConnectionObject = findActiveConnection(activatedPath);

        ui->connectionShowPage->updateActivatedConnectionInfo(activatedPath);
        ui->connectionShowPage->update();
        KLOG_DEBUG() << "activate Connection successed";
    }
}
