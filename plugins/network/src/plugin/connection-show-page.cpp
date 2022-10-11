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

#include "connection-show-page.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <style-property.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include "animation-loading-label.h"
#include "ui_connection-show-page.h"
using namespace NetworkManager;

ConnectionShowPage::ConnectionShowPage(QWidget* parent) : QWidget(parent), ui(new Ui::ConnectionShowPage)
{
    ui->setupUi(this);
}

ConnectionShowPage::~ConnectionShowPage()
{
    delete ui;
}

void ConnectionShowPage::setTitle(QString title)
{
    ui->title->setText(title);
}

void ConnectionShowPage::init(NetworkManager::ConnectionSettings::ConnectionType connectionType, const QString& devicePath)
{
    m_connectionType = connectionType;
    m_devicePath = devicePath;
    ui->connectionLists->setDevicePath(devicePath);
    ui->connectionLists->setItemWidgetType(ITEM_WIDGET_TYPE_PLUGIN);

    initUI();
    initConnect();
}

// TODO:增加判断设备是否可用的逻辑,unmanager 和 unavalibale的处理
void ConnectionShowPage::initUI()
{
    // switchButton Checked 默认为false
    m_switchButton = new KiranSwitchButton(this);
    m_switchButton->setAccessibleName(QString("SwitchEnableWireless"));
    initSwitchButton();

    ui->titleLayout->addWidget(m_switchButton);
    ui->connectionLists->setMaximumHeight(ui->connectionLists->sizeHintForRow(0) * ui->connectionLists->count() + (2 * ui->connectionLists->frameWidth()));
    Kiran::StylePropertyHelper::setButtonType(ui->createConnectionButton, Kiran::BUTTON_Default);
    ui->createConnectionButton->setIcon(QIcon(":/kcp-network-images/connection-add.svg"));

    if (m_connectionType == ConnectionSettings::Wireless)
    {
        ui->connectionLists->showWirelessNetworkLists();
        ui->connectionLists->setVisible(isWirelessEnabled());
    }
    else
        ui->connectionLists->showConnectionLists(m_connectionType);
}

void ConnectionShowPage::initConnect()
{
    connect(ui->createConnectionButton, &QPushButton::clicked, [=]()
            { emit requestCreatConnection(); });
    connect(ui->connectionLists, &ConnectionLists::requestCreatConnection, this, &ConnectionShowPage::requestCreatConnection);

    connect(ui->connectionLists, &ConnectionLists::requestEditConnection, this, &ConnectionShowPage::requestEditConnection);
    connect(ui->connectionLists, &ConnectionLists::requestActivateCurrentItemConnection, this, &ConnectionShowPage::requestActivateCurrentItemConnection);
    connect(ui->connectionLists, &ConnectionLists::requestConnectWirelessNetwork, this, &ConnectionShowPage::requestConnectWirelessNetwork);
    connect(ui->connectionLists, &ConnectionLists::deactivatedItemConnection, this, &ConnectionShowPage::deactivatedItemConnection);
    connect(ui->connectionLists, &ConnectionLists::connectionUpdated, this, &ConnectionShowPage::connectionUpdated);

    connect(ui->connectionLists, &ConnectionLists::sendSsidToWireless, this, &ConnectionShowPage::sendSsidToWireless);

    connect(m_switchButton, &KiranSwitchButton::toggled, this, &ConnectionShowPage::handleToggledSwitchButton, Qt::UniqueConnection);

    /**
     *如果频繁的打开/禁用网络，会频繁发送信号，使用定时器进行处理
     * */
    // m_timer.setInterval(200);
    // m_timer.setSingleShot(true);
    // connect(notifier(), &Notifier::wirelessEnabledChanged, this, [this](bool enable)
    //         {
    //             m_wirlessNetworkEnable = enable;
    //             m_timer.start(); });

    // connect(&m_timer, &QTimer::timeout, this, [this]()
    //         { handleWirelessEnabledChanged(m_wirlessNetworkEnable); });

    if (m_connectionType == ConnectionSettings::Wireless)
    {
        connect(notifier(), &Notifier::wirelessEnabledChanged, this, &ConnectionShowPage::handleWirelessEnabledChanged, Qt::UniqueConnection);
    }

    connect(notifier(), &Notifier::networkingEnabledChanged, [=]() {});
}

//弃用
void ConnectionShowPage::setConnectionType(ConnectionSettings::ConnectionType connectionType)
{
    m_connectionType = connectionType;
}

void ConnectionShowPage::initSwitchButton()
{
    switch (m_connectionType)
    {
    case ConnectionSettings::Wired:
        break;
    case ConnectionSettings::Wireless:
        m_switchButton->setChecked(isWirelessEnabled());
        break;
    case ConnectionSettings::Vpn:
        break;
    default:
        break;
    }
}

void ConnectionShowPage::handleToggledSwitchButton(bool toggled)
{
    KLOG_DEBUG() << "--------------------------------handleToggledSwitchButton:" << toggled;
    switch (m_connectionType)
    {
    case ConnectionSettings::Wired:
        ui->connectionLists->setVisible(toggled);
        // ui->createConnectionButton->setVisible(toggled);
        break;
    case ConnectionSettings::Wireless:
        setWirelessEnabled(toggled);
        break;
    case ConnectionSettings::Vpn:
        break;
    default:
        break;
    }
}

void ConnectionShowPage::handleWirelessEnabledChanged(bool enabled)
{
    KLOG_DEBUG() << "*****************************handleWirelessEnabledChanged:" << enabled;
    //处理通过命令行等其他方式禁用无线网络的情况
    m_switchButton->blockSignals(true);
    m_switchButton->setChecked(enabled);
    m_switchButton->blockSignals(false);
    ui->connectionLists->setVisible(enabled);
}

//弃用
void ConnectionShowPage::setDevicePath(const QString& devicePath)
{
    m_devicePath = devicePath;
    ui->connectionLists->setDevicePath(devicePath);
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
    ui->connectionLists->addConnectionToLists(ptr, devicePath);
}

void ConnectionShowPage::showWirelessNetworkLists()
{
    ui->connectionLists->showWirelessNetworkLists();
}

void ConnectionShowPage::addWirelessNetworkToLists(WirelessNetwork::Ptr network, const QString& devicePath)
{
    ui->connectionLists->addWirelessNetworkToLists(network, devicePath);
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

QListWidgetItem* ConnectionShowPage::findItemByUuid(const QString& uuid)
{
    return ui->connectionLists->findItemByUuid(uuid);
}

QListWidgetItem* ConnectionShowPage::findItemBySsid(const QString& ssid)
{
    return ui->connectionLists->findItemBySsid(ssid);
}

QListWidgetItem* ConnectionShowPage::findItemByActivatedPath(const QString& activatedPath)
{
    return ui->connectionLists->findItemByActivatedPath(activatedPath);
}

void ConnectionShowPage::updateItemActivatingStatus(QListWidgetItem* item)
{
    ui->connectionLists->updateItemActivatingStatus(item);
}

void ConnectionShowPage::updateItemActivatedStatus(const QString& activatedPath)
{
    ui->connectionLists->updateItemActivatedStatus(activatedPath);
}

//弃用
void ConnectionShowPage::setItemWidgetType(ItemWidgetType itemType)
{
    ui->connectionLists->setItemWidgetType(itemType);
}

void ConnectionShowPage::handleActiveStateDeactivated(const QString& activatedConnectionPath)
{
    ui->connectionLists->handleActiveStateDeactivated(activatedConnectionPath);
}
void ConnectionShowPage::sortItems()
{
    ui->connectionLists->sortItems();
}

void ConnectionShowPage::updateItemActivatedPath(QListWidgetItem* item, QString activatedPath)
{
    ui->connectionLists->updateItemActivatedPath(item, activatedPath);
}

void ConnectionShowPage::itemSimpleStatus(QListWidgetItem* item)
{
    ui->connectionLists->itemSimpleStatus(item);
}

QListWidgetItem* ConnectionShowPage::item(int row)
{
    return ui->connectionLists->item(row);
}
int ConnectionShowPage::count()
{
    return ui->connectionLists->count();
}
