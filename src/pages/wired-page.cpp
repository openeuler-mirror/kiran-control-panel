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

WiredPage::WiredPage(QWidget *parent) : Page(parent), ui(new Ui::WiredPage)
{
    ui->setupUi(this);
    initUI();
    getDeviceInfo();
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
    showWiredConnections();
}

void WiredPage::getDeviceInfo()
{
    const Device::List deviceList = networkInterfaces();
    KLOG_DEBUG() << "deviceList:" << deviceList;
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == Device::Ethernet)
        {
            m_wiredDevice = qobject_cast<WiredDevice *>(dev);
            KLOG_DEBUG() << "m_wiredDevice->ipInterfaceName():" << m_wiredDevice->ipInterfaceName();
            KLOG_DEBUG() << "m_wiredDevice->interfaceName():" << m_wiredDevice->interfaceName();
            KLOG_DEBUG() << "m_wiredDevice->uni():" << m_wiredDevice->uni();
            KLOG_DEBUG() << "m_wiredDevice->udi():" << m_wiredDevice->udi();

            m_deviceMap.insert(m_wiredDevice->permanentHardwareAddress(), m_wiredDevice->uni());
        }
    }
    KLOG_DEBUG() << "m_deviceMap:" << m_deviceMap;
    if (m_deviceMap.isEmpty())
    {
        KLOG_DEBUG() << "Wired device not found";
    }
}

void WiredPage::showWiredConnections()
{
    const Connection::List connectionList = NetworkManager::listConnections();
    for (Connection::Ptr conn : connectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
        {
            ui->connectionShowPage->addConnectionToLists(conn);
        }
    }
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

    connect(ui->returnButton, &QPushButton::clicked, [=]() {
        ui->wiredSettingPage->clearPtr();
        ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
    });
    //    connect(ui->saveButton, &QPushButton::clicked, ui->wiredSettingPage, &WiredSettingPage::handleSaveButtonClicked);

    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        ui->wiredSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wired);
    });

    connect(ui->wiredSettingPage, &WiredSettingPage::settingUpdated, [=]() {
        KLOG_DEBUG() << "WiredSettingPage::settingUpdated";
        ui->wiredSettingPage->clearPtr();
        refreshConnectionLists();
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestActivateCurrentItemConnection,
            this, &WiredPage::handleActivateConnection);

    initNotifierConnection();

    //检测到新设备时，刷新
    connect(notifier(), &Notifier::deviceAdded, [=](const QString &uni) {
        getDeviceInfo();
    });

    connect(notifier(), &Notifier::deviceRemoved, [=](const QString &uni) {
        getDeviceInfo();
    });
}

void WiredPage::refreshConnectionLists()
{
    KLOG_DEBUG() << "WiredPage::refreshConnectionLists()";
    ui->connectionShowPage->clearConnectionLists();
    showWiredConnections();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void WiredPage::handleActivateConnection(QString connectionPath)
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
        KLOG_DEBUG() << "activate connection failed" << reply.error();
        //TODO:连接失败的处理
    }
    else
    {
        QString activatedPath = reply.value().path();
        ActiveConnection::Ptr activatedConnectionObject = findActiveConnection(activatedPath);
        ui->connectionShowPage->updateActivatedConnectionInfo(activatedPath);
        ui->connectionShowPage->update();
        KLOG_DEBUG() << "activate Connection successed";
    }
}

void WiredPage::handleNotifierConnectionChanged()
{
    ui->wiredSettingPage->clearPtr();
    refreshConnectionLists();
}
