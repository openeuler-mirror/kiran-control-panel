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
#include "wireless-manager.h"
#include <qt5-log-i.h>
#include <style-property.h>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <QEventLoop>
#include <QPointer>
#include <QScrollBar>
#include "signal-forward.h"
#include "status-notification.h"
#include "text-input-dialog.h"
#include "ui_wireless-manager.h"

using namespace NetworkManager;

WirelessManager::WirelessManager(const QString &devicePath, QWidget *parent) : Manager(parent), ui(new Ui::WirelessManager)
{
    ui->setupUi(this);

    ui->wirelessDeviceList->init(Device::Wifi);

    initUI();
    initConnection();
}

WirelessManager::~WirelessManager()
{
    delete ui;
}

void WirelessManager::initUI()
{
}

void WirelessManager::initConnection()
{
    connect(SignalForward::instance(), &SignalForward::wirelessConnectionEdited, this, &WirelessManager::editConnection);

    connect(ui->returnButton, &QPushButton::clicked, this, &WirelessManager::returnPreviousPage);
    connect(ui->saveButton, &QPushButton::clicked, this, [this]()
            {
                ui->wirelessSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wireless);
                returnPreviousPage(); });

    connect(ui->wirelessSettingPage, &WirelessSettingPage::returnPreviousPage, this, &WirelessManager::returnPreviousPage);
}

void WirelessManager::editConnection(const QString &uuid, QString activeConnectionPath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activeConnectionPath);
    if (activeConnection.isNull())
    {
        return;
    }
    ConnectionSettings::Ptr connectionSettings = activeConnection->connection()->settings();
    ui->wirelessSettingPage->setConnection(activeConnection->connection());
    ui->wirelessSettingPage->setConnectionSettings(connectionSettings);
    ui->wirelessSettingPage->initSettingPage();
    ui->wirelessSettingPage->showSettingPage(activeConnectionPath);
    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void WirelessManager::returnPreviousPage()
{
    ui->wirelessSettingPage->clearPtr();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}
