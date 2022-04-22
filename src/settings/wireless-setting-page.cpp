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

#include "wireless-setting-page.h"
#include "ui_wireless-setting-page.h"
#include <qt5-log-i.h>

WirelessSettingPage::WirelessSettingPage(QWidget *parent) : SettingPage(parent), ui(new Ui::WirelessSettingPage)
{
    ui->setupUi(this);
    initConnection();
    KLOG_DEBUG() << "WirelessSettingPage::WirelessSettingPage";
}

WirelessSettingPage::~WirelessSettingPage()
{
    delete ui;
    KLOG_DEBUG() << "WirelessSettingPage::~WirelessSettingPage()";
}

void WirelessSettingPage::initConnection()
{
    connect(ui->generalButton,&GeneralButton::disconnectButtonClicked,this,&WirelessSettingPage::returnPreviousPage);
    connect(ui->generalButton,&GeneralButton::deleteButtonClicked,this,&WirelessSettingPage::returnPreviousPage);
}

void WirelessSettingPage::initSettingPage()
{
    initSpecificSettings();
    initWidgets();
}

void WirelessSettingPage::initSpecificSettings()
{
    m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
    m_ipv6Setting = m_connectionSettings->setting(Setting::SettingType::Ipv6).dynamicCast<Ipv6Setting>();
    m_wirelessSetting = m_connectionSettings->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
    m_wirelessSecuritySetting = m_connectionSettings->setting(Setting::SettingType::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();
}

void WirelessSettingPage::initWidgets()
{
    ui->generalWidget->setConnectionSettings(m_connectionSettings);
    ui->ipv4Widget->setIpv4Setting(m_ipv4Setting);
    ui->ipv6Widget->setIpv6Setting(m_ipv6Setting);
    ui->generalButton->setConnectionPtr(m_connection);
    ui->wirelessSecurity->setWirelessSecuritySetting(m_wirelessSecuritySetting);
    ui->wireless->setWirelessSetting(m_wirelessSetting);
}

void WirelessSettingPage::showSettingPage(QString activeConnectionPath)
{
    ui->generalWidget->setNameLabel(tr("Wireless name"));
    ui->generalWidget->showSettings(ConnectionSettings::ConnectionType::Wired);
    ui->ipv4Widget->showSettings();
    ui->ipv6Widget->showSettings();
    ui->wirelessSecurity->showSettings();
    ui->wireless->showSettings();

    if (m_connectionSettings.isNull())
    {
        ui->generalButton->initButton(SETTING_CONNECTION_STATUS_NEW);
    }
    else
    {
        //通过将激活路径传入SettingPage,判断该连接是否激活，也可通过uuid判断
        if (activeConnectionPath.isEmpty())
            ui->generalButton->initButton(SETTING_CONNECTION_STATUS_DEACTIVATED);
        else
            ui->generalButton->initButton(SETTING_CONNECTION_STATUS_ACTIVATED,activeConnectionPath);
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
    ui->generalButton->clearPtr();
    ui->ipv4Widget->clearPtr();
    ui->ipv6Widget->clearPtr();
    ui->wirelessSecurity->clearPtr();
    ui->wireless->clearPtr();
}


