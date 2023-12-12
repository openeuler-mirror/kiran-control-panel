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

#include "wireless-setting-page.h"
#include <qt5-log-i.h>
#include "ui_wireless-setting-page.h"
#include "logging-category.h"
using namespace NetworkManager;

WirelessSettingPage::WirelessSettingPage(QWidget *parent) : SettingPage(parent), ui(new Ui::WirelessSettingPage)
{
    ui->setupUi(this);
    initConnection();
    KLOG_DEBUG(qLcNetwork) << "WirelessSettingPage::WirelessSettingPage";
}

WirelessSettingPage::~WirelessSettingPage()
{
    delete ui;
    KLOG_DEBUG(qLcNetwork) << "WirelessSettingPage::~WirelessSettingPage()";
}

void WirelessSettingPage::initConnection()
{
    connect(ui->disconnectAndDeleteButton, &DisconnectAndDeleteButton::disconnectButtonClicked, this, &WirelessSettingPage::returnPreviousPage);
    connect(ui->disconnectAndDeleteButton, &DisconnectAndDeleteButton::deleteButtonClicked, this, &WirelessSettingPage::returnPreviousPage);
}

void WirelessSettingPage::initSettingPage()
{
    initSpecificSettings();
}

void WirelessSettingPage::initSpecificSettings()
{
    m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
    m_ipv6Setting = m_connectionSettings->setting(Setting::SettingType::Ipv6).dynamicCast<Ipv6Setting>();
    m_wirelessSetting = m_connectionSettings->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
    m_wirelessSecuritySetting = m_connectionSettings->setting(Setting::SettingType::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();

    m_ipv4Setting->setInitialized(true);
    m_ipv6Setting->setInitialized(true);
    m_wirelessSetting->setInitialized(true);
    m_wirelessSecuritySetting->setInitialized(true);

    ui->generalWidget->setConnectionSettings(m_connectionSettings);
    ui->ipv4Widget->setIpv4Setting(m_ipv4Setting);
    ui->ipv6Widget->setIpv6Setting(m_ipv6Setting);
    ui->disconnectAndDeleteButton->setConnectionPtr(m_connection);
    ui->wirelessSecurity->setWirelessSecuritySetting(m_wirelessSecuritySetting);
    ui->wireless->setWirelessSetting(m_wirelessSetting);
}

void WirelessSettingPage::showSettingPage(QString activeConnectionPath)
{
    ui->generalWidget->setNameLabel(tr("Wireless name"));
    ui->generalWidget->showSettings(ConnectionSettings::ConnectionType::Wireless);
    ui->ipv4Widget->showSettings();
    ui->ipv6Widget->showSettings();
    ui->wirelessSecurity->showSettings();
    ui->wireless->showSettings();

    if (m_connectionSettings.isNull())
    {
        ui->disconnectAndDeleteButton->initButton(SETTING_CONNECTION_STATUS_NEW);
        ui->disconnectAndDeleteButton->setVisible(false);
    }
    else
    {
        ui->disconnectAndDeleteButton->setVisible(true);
        //通过将激活路径传入SettingPage,判断该连接是否激活，也可通过uuid判断
        if (activeConnectionPath.isEmpty())
            ui->disconnectAndDeleteButton->initButton(SETTING_CONNECTION_STATUS_DEACTIVATED);
        else
            ui->disconnectAndDeleteButton->initButton(SETTING_CONNECTION_STATUS_ACTIVATED, activeConnectionPath);
    }
}

void WirelessSettingPage::saveSettingPage()
{
    ui->generalWidget->saveSettings();
    ui->ipv4Widget->saveSettings();
    ui->ipv6Widget->saveSettings();
    ui->wirelessSecurity->saveSettings();
    ui->wireless->saveSettings();
}

void WirelessSettingPage::clearPtr()
{
    m_ipv4Setting.clear();
    m_ipv6Setting.clear();
    m_wirelessSetting.clear();
    m_wirelessSecuritySetting.clear();
    SettingPage::clearPtr();

    ui->generalWidget->clearPtr();
    ui->disconnectAndDeleteButton->clearPtr();
    ui->ipv4Widget->clearPtr();
    ui->ipv6Widget->clearPtr();
    ui->wirelessSecurity->clearPtr();
    ui->wireless->clearPtr();
}
