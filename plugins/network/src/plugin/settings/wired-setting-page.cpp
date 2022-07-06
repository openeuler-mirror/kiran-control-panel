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

#include "wired-setting-page.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include "ui_wired-setting-page.h"

WiredSettingPage::WiredSettingPage(QWidget *parent) : SettingPage(parent), ui(new Ui::WiredSettingPage)
{
    ui->setupUi(this);
    initConnecton();
    KLOG_DEBUG() << "WiredSettingPage::WiredSettingPage(QWidget *parent)";
}

WiredSettingPage::~WiredSettingPage()
{
    KLOG_DEBUG() << "WiredSettingPage::~WiredSettingPage()";
    delete ui;
}

void WiredSettingPage::initSettingPage()
{
    initSpecificSettings();
    initWidgets();
}

void WiredSettingPage::initConnecton()
{
    connect(ui->disconnectAndDeleteButton, &DisconnectAndDeleteButton::disconnectButtonClicked, this, &WiredSettingPage::returnPreviousPage);
    connect(ui->disconnectAndDeleteButton, &DisconnectAndDeleteButton::deleteButtonClicked, this, &WiredSettingPage::returnPreviousPage);
}

void WiredSettingPage::initSpecificSettings()
{
    m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
    m_ipv6Setting = m_connectionSettings->setting(Setting::SettingType::Ipv6).dynamicCast<Ipv6Setting>();
    m_wiredSetting = m_connectionSettings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
}

void WiredSettingPage::initWidgets()
{
    ui->connectionNameWidget->setConnectionSettings(m_connectionSettings);
    ui->ipv4Widget->setIpv4Setting(m_ipv4Setting);
    ui->ipv6Widget->setIpv6Setting(m_ipv6Setting);
    ui->ethernetWidget->setWiredSetting(m_wiredSetting);
    ui->disconnectAndDeleteButton->setConnectionPtr(m_connection);
}

void WiredSettingPage::showSettingPage(QString activeConnectionPath)
{
    ui->connectionNameWidget->setNameLabel(tr("Network name"));
    ui->connectionNameWidget->showSettings(ConnectionSettings::ConnectionType::Wired);
    ui->ipv4Widget->showSettings();
    ui->ipv6Widget->showSettings();
    ui->ethernetWidget->showSettings();

    if (m_connectionSettings.isNull())
    {
        ui->disconnectAndDeleteButton->initButton(SETTING_CONNECTION_STATUS_NEW);
    }
    else
    {
        // 通过将激活路径传入SettingPage,判断该连接是否激活，也可通过uuid判断
        if (activeConnectionPath.isEmpty())
            ui->disconnectAndDeleteButton->initButton(SETTING_CONNECTION_STATUS_DEACTIVATED);
        else
            ui->disconnectAndDeleteButton->initButton(SETTING_CONNECTION_STATUS_ACTIVATED, activeConnectionPath);
    }
}

void WiredSettingPage::saveSettingPage()
{
    ui->connectionNameWidget->saveSettings();
    ui->ipv4Widget->saveSettings();
    ui->ipv6Widget->saveSettings();
    ui->ethernetWidget->saveSettings();
}

void WiredSettingPage::clearPtr()
{
    m_ipv4Setting.clear();
    m_ipv6Setting.clear();
    m_wiredSetting.clear();
    SettingPage::clearPtr();

    ui->connectionNameWidget->clearPtr();
    ui->disconnectAndDeleteButton->clearPtr();
    ui->ipv4Widget->clearPtr();
    ui->ipv6Widget->clearPtr();
    ui->ethernetWidget->clearPtr();
}

bool WiredSettingPage::isInputValid()
{
    if (ui->ipv4Widget->isInputValid() || ui->ipv6Widget->isInputValid() || ui->connectionNameWidget->isInputValid())
        return true;
    else
        return false;
}
