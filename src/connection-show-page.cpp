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

#include "connection-show-page.h"
#include <kiran-switch-button.h>
#include <libnotify/notify.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include "animation-loading-label.h"
#include "ui_connection-show-page.h"

ConnectionShowPage::ConnectionShowPage(QWidget* parent) : QWidget(parent), ui(new Ui::ConnectionShowPage)
{
    ui->setupUi(this);
    initUI();
    initConnect();
}

ConnectionShowPage::~ConnectionShowPage()
{
    KLOG_DEBUG() << "ConnectionShowPage::~ConnectionShowPage()";
    delete ui;
}

void ConnectionShowPage::setTitle(QString title)
{
    ui->title->setText(title);
}

void ConnectionShowPage::initUI()
{
    m_switchButton = new KiranSwitchButton(this);
    ui->titleLayout->addWidget(m_switchButton);
    ui->connectionLists->setMaximumHeight(ui->connectionLists->sizeHintForRow(0) * ui->connectionLists->count() + (2 * ui->connectionLists->frameWidth()));
}

void ConnectionShowPage::initConnect()
{
    connect(ui->createConnectionButton, &QPushButton::clicked, [=]() { emit requestCreatConnection(); });
    connect(ui->connectionLists, &ConnectionLists::requestCreatConnection, this, &ConnectionShowPage::requestCreatConnection);
//    connect(ui->connectionLists, &QListWidget::itemClicked, this, &ConnectionShowPage::handleConnectionItemClicked);
    connect(ui->connectionLists, &ConnectionLists::requestEditConnection, this, &ConnectionShowPage::requestEditConnection);
    connect(ui->connectionLists, &ConnectionLists::requestActivateCurrentItemConnection, this, &ConnectionShowPage::requestActivateCurrentItemConnection);
    connect(ui->connectionLists, &ConnectionLists::requestConnectWirelessNetwork, this, &ConnectionShowPage::requestConnectWirelessNetwork);
    connect(ui->connectionLists, &ConnectionLists::deactivatedItemConnection, this, &ConnectionShowPage::deactivatedItemConnection);
    connect(ui->connectionLists, &ConnectionLists::connectionUpdated, this, &ConnectionShowPage::connectionUpdated);
}

void ConnectionShowPage::handleConnectionItemClicked(QListWidgetItem* item)
{
    ui->connectionLists->handleConnectionItemClicked(item);
}

void ConnectionShowPage::setSwitchButtonVisible(bool visible)
{
    m_switchButton->setVisible(visible);
}

void ConnectionShowPage::setCreateButtonVisible(bool visible)
{
    ui->createConnectionButton->setVisible(visible);
}

void ConnectionShowPage::showConnectionLists(ConnectionSettings::ConnectionType type)
{
    ui->connectionLists->showConnectionLists(type);
}

void ConnectionShowPage::addConnectionToLists(Connection::Ptr ptr, const QString& devicePath)
{
    ui->connectionLists->addConnectionToLists(ptr,devicePath);
}

void ConnectionShowPage::showWirelessNetworkLists()
{
    ui->connectionLists->showWirelessNetworkLists();
}

void ConnectionShowPage::addWirelessNetworkToLists(WirelessNetwork::Ptr network, const QString& devicePath)
{
    ui->connectionLists->addWirelessNetworkToLists(network,devicePath);
}

void ConnectionShowPage::removeConnectionFromLists(const QString& path)
{
    ui->connectionLists->removeConnectionFromLists(path);
}

void ConnectionShowPage::removeWirelessNetworkFromLists(const QString& ssid)
{
    ui->connectionLists->removeWirelessNetworkFromLists(ssid);
}

void ConnectionShowPage::clearConnectionLists()
{
    ui->connectionLists->clearConnectionLists();
}

int ConnectionShowPage::findItemByUuid(const QString& uuid)
{
    return ui->connectionLists->findItemByUuid(uuid);
}

int ConnectionShowPage::findItemBySsid(const QString& ssid)
{
    return ui->connectionLists->findItemBySsid(ssid);
}

void ConnectionShowPage::setCurrentActiveItem(int row)
{
    ui->connectionLists->setCurrentActiveItem(row);
}
void ConnectionShowPage::connectionItemLoadingAnimation()
{
    ui->connectionLists->updateItemActivatingStatus();
}

void ConnectionShowPage::updateActivatedConnectionInfo(QString activatedPath)
{
    ui->connectionLists->updateItemActivatedStatus(activatedPath);
}

void ConnectionShowPage::clearDeactivatedConnectionInfo(const QString& deactivatedPath)
{
    int row = ui->connectionLists->findItemByActivatedPath(deactivatedPath);
    auto activatedItem = ui->connectionLists->item(row);
    ui->connectionLists->clearItemActivatedConnectionInfo(activatedItem);
}

void ConnectionShowPage::connectionStateNotify(ActiveConnection::State state,const QString &activatedConnectionPath)
{
//    ui->connectionLists->connectionStateNotify(state,activatedConnectionPath);
}