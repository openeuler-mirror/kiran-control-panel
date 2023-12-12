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

#include "wired-setting-page.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include "kiran-tips/kiran-tips.h"
#include "ui_wired-setting-page.h"
#include <NetworkManagerQt/WiredDevice>
#include "logging-category.h"

using namespace NetworkManager;

WiredSettingPage::WiredSettingPage(QWidget *parent) : SettingPage(parent), ui(new Ui::WiredSettingPage)
{
    ui->setupUi(this);
    initConnecton();
    m_errorTip = new KiranTips(this);
    m_errorTip->setAnimationEnable(true);
    m_errorTip->setShowPosition(KiranTips::POSITION_BOTTM);
    ui->connectionNameWidget->setErrorTips(m_errorTip);
    ui->ipv4Widget->setErrorTips(m_errorTip);
    ui->ipv6Widget->setErrorTips(m_errorTip);
    ui->ethernetWidget->setErrorTips(m_errorTip);
}

WiredSettingPage::~WiredSettingPage()
{
    delete ui;
}

void WiredSettingPage::initSettingPage()
{
    initSpecificSettings();
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

    m_ipv4Setting->setInitialized(true);
    m_ipv6Setting->setInitialized(true);
    m_wiredSetting->setInitialized(true);

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

void WiredSettingPage::createSettingPage(const QString &devicePath)
{
    showSettingPage();
    
    auto device = findNetworkInterface(devicePath);
    WiredDevice::Ptr wiredDevice = qobject_cast<WiredDevice *>(device);
    QString macAddress = wiredDevice->permanentHardwareAddress();
    if(macAddress.isEmpty())
    {
        macAddress = wiredDevice->hardwareAddress();
    }
    KLOG_DEBUG(qLcNetwork) << "binding MAC Address:" << macAddress;
    ui->ethernetWidget->setDefaultMacAddress(macAddress);
}

bool WiredSettingPage::isInputValid()
{
    if (ui->ipv4Widget->isInputValid() && 
        ui->ipv6Widget->isInputValid() &&
        ui->connectionNameWidget->isInputValid() && 
        ui->ethernetWidget->isInputValid())
        return true;
    else
        return false;
}
