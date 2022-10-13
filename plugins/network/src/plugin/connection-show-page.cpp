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
#include <qt5-log-i.h>
#include <style-property.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include "animation-loading-label.h"
#include "ui_connection-show-page.h"
using namespace NetworkManager;

#define PLUGIN_ITEM_WIDGET_HEIGHT 36

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
    ui->connectionList->setDevicePath(devicePath);

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

    ui->connectionList->setMaximumHeight((ui->connectionList->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) *
                                         ui->connectionList->count());

    Kiran::StylePropertyHelper::setButtonType(ui->createConnectionButton, Kiran::BUTTON_Default);
    ui->createConnectionButton->setIcon(QIcon(":/kcp-network-images/connection-add.svg"));

    if (m_connectionType == ConnectionSettings::Wireless)
    {
        ui->connectionList->showWirelessNetworkList();
        ui->connectionList->setVisible(isWirelessEnabled());
    }
    else
        ui->connectionList->showConnectionList(m_connectionType);
}

void ConnectionShowPage::initConnect()
{
    connect(ui->createConnectionButton, &QPushButton::clicked, [=]()
            { emit creatConnection(); });

    connect(ui->connectionList, &PluginConnectionList::creatConnection, this, &ConnectionShowPage::creatConnection);
    connect(ui->connectionList, &PluginConnectionList::editConnection, this, &ConnectionShowPage::editConnection);
    connect(ui->connectionList, &PluginConnectionList::activateSelectedConnection, this, &ConnectionShowPage::activateSelectedConnection);
    connect(ui->connectionList, &PluginConnectionList::activateSelectedWirelessNetwork, this, &ConnectionShowPage::activateSelectedWirelessNetwork);
    connect(ui->connectionList, &PluginConnectionList::connectionUpdated, this, &ConnectionShowPage::connectionUpdated);

    connect(ui->connectionList, &PluginConnectionList::sendSsidToWireless, this, &ConnectionShowPage::sendSsidToWireless);

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
    switch (m_connectionType)
    {
    case ConnectionSettings::Wired:
        ui->connectionList->setVisible(toggled);
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
#define PLUGIN_ITEM_WIDGET_HEIGHT 36
void ConnectionShowPage::handleWirelessEnabledChanged(bool enabled)
{
    KLOG_DEBUG() << "Wireless Enabled Changed:" << enabled;
    //处理通过命令行等其他方式禁用无线网络的情况
    m_switchButton->blockSignals(true);
    m_switchButton->setChecked(enabled);
    m_switchButton->blockSignals(false);
    ui->connectionList->setVisible(enabled);
}

void ConnectionShowPage::setSwitchButtonVisible(bool visible)
{
    m_switchButton->setVisible(visible);
}

void ConnectionShowPage::setCreateButtonVisible(bool visible)
{
    ui->createConnectionButton->setVisible(visible);
}

void ConnectionShowPage::showConnectionList(ConnectionSettings::ConnectionType type)
{
    ui->connectionList->showConnectionList(type);
}

void ConnectionShowPage::addConnection(Connection::Ptr ptr, const QString& devicePath)
{
    ui->connectionList->addConnection(ptr, devicePath);
}

void ConnectionShowPage::showWirelessNetworkList()
{
    ui->connectionList->showWirelessNetworkList();
}

void ConnectionShowPage::addWirelessNetwork(WirelessNetwork::Ptr network, const QString& devicePath)
{
    ui->connectionList->addWirelessNetwork(network, devicePath);
}

void ConnectionShowPage::removeConnectionFromList(const QString& path)
{
    ui->connectionList->removeConnectionFromList(path);
    ui->connectionList->setMaximumHeight((ui->connectionList->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * ui->connectionList->count());
}

void ConnectionShowPage::removeWirelessNetworkFromList(const QString& ssid)
{
    ui->connectionList->removeWirelessNetworkFromList(ssid);
    ui->connectionList->setMaximumHeight((ui->connectionList->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * ui->connectionList->count());
}

void ConnectionShowPage::clearConnectionList()
{
    ui->connectionList->clearConnectionList();
}

QWidget* ConnectionShowPage::findItemWidgetByUuid(const QString& uuid)
{
    return ui->connectionList->findItemWidgetByUuid(uuid);
}

QWidget* ConnectionShowPage::findItemWidgetBySsid(const QString& ssid)
{
    return ui->connectionList->findItemWidgetBySsid(ssid);
}

QWidget* ConnectionShowPage::findItemWidgetByActivePath(const QString& activePath)
{
    return ui->connectionList->findItemWidgetByActivePath(activePath);
}

void ConnectionShowPage::setItemWidgetStatus(const QString& activePath, NetworkManager::ActiveConnection::State state)
{
    ui->connectionList->setItemWidgetStatus(activePath, state);
}

void ConnectionShowPage::handleActiveStateDeactivated(const QString& activatedConnectionPath)
{
    ui->connectionList->handleActiveStateDeactivated(activatedConnectionPath);
}
void ConnectionShowPage::sort()
{
    ui->connectionList->sort();
}

void ConnectionShowPage::updateItemWidgetActivePath(QWidget* widget, QString activatedPath)
{
    ui->connectionList->updateItemWidgetActivePath(widget, activatedPath);
}

int ConnectionShowPage::count()
{
    return ui->connectionList->count();
}
