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
#include "wired-manager.h"
#include <libnm/nm-connection.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QPointer>
#include <QScrollBar>
#include "device-list.h"
#include "logging-category.h"
#include "signal-forward.h"
#include "status-notification.h"
#include "ui_wired-manager.h"

using namespace NetworkManager;

WiredManager::WiredManager(const QString &devicePath, QWidget *parent) : Manager(parent), ui(new Ui::WiredManager)
{
    ui->setupUi(this);
    ui->wiredDeviceList->init(Device::Ethernet);
    initUI();
    initConnection();
}

WiredManager::~WiredManager()
{
    ui->wiredSettingPage->clearPtr();
    delete ui;
}

void WiredManager::initUI()
{
    // FIXME: 后续使用新版kiran-integration-qt5中提供的setButtonType函数
    // Kiran::StylePropertyHelper::setButtonType(ui->saveButton, Kiran::BUTTON_Default);
}

void WiredManager::initConnection()
{
    connect(ui->returnButton, &QPushButton::clicked, this, &WiredManager::returnPreviousPage);
    connect(ui->saveButton, &QPushButton::clicked, this, &WiredManager::saveConnectionSettings);
    connect(ui->wiredSettingPage, &WiredSettingPage::returnPreviousPage, this, &WiredManager::returnPreviousPage);

    connect(SignalForward::instance(), &SignalForward::wiredConnectionEdited, this, &WiredManager::editConnection);
    connect(SignalForward::instance(), &SignalForward::createConnection, this, &WiredManager::creatConnection);
}

void WiredManager::creatConnection(const QString &devicePath)
{
    ui->wiredSettingPage->createSettingPage(devicePath);
    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void WiredManager::editConnection(const QString &uuid, QString activeConnectionPath)
{
    ui->wiredSettingPage->initConnectionSettings(ConnectionSettings::ConnectionType::Wired, uuid);
    ui->wiredSettingPage->initSettingPage();
    ui->wiredSettingPage->showSettingPage(activeConnectionPath);
    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void WiredManager::returnPreviousPage()
{
    ui->wiredSettingPage->clearPtr();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void WiredManager::saveConnectionSettings()
{
    if (ui->wiredSettingPage->isInputValid())
    {
        ui->wiredSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wired);
        returnPreviousPage();
    }
    else
    {
        KLOG_DEBUG(qLcNetwork) << "Invalid input exists";
    }
}
