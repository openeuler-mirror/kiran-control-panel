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

#include "vpn-l2tp-setting.h"
#include "ui_vpn-l2tp-setting.h"
#include <NetworkManagerQt/Settings>
#include <qt5-log-i.h>
#define ServiceTypeL2TP "org.freedesktop.NetworkManager.l2tp"

VpnL2tpSetting::VpnL2tpSetting(QWidget *parent) : SettingPage(parent), ui(new Ui::VpnL2tpSetting)
{
    ui->setupUi(this);
    ui->generalWidget->setNameLabel(tr("VPN name"));
    initConnection();
}

VpnL2tpSetting::~VpnL2tpSetting()
{
    delete ui;
}

void VpnL2tpSetting::initConnection()
{
    connect(ui->generalButton,&DisconnectAndDeleteButton::disconnectButtonClicked,this,&VpnL2tpSetting::returnPreviousPage);
    connect(ui->generalButton,&DisconnectAndDeleteButton::deleteButtonClicked,this,&VpnL2tpSetting::returnPreviousPage);
}

void VpnL2tpSetting::initSettingPage()
{
    initSpecificSettings();
    initWidgets();
}

void VpnL2tpSetting::initSpecificSettings()
{
    m_vpnSetting = m_connectionSettings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
    m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
    m_vpnSetting->setServiceType(ServiceTypeL2TP);
}

void VpnL2tpSetting::initWidgets()
{
    ui->generalWidget->setConnectionSettings(m_connectionSettings);
    ui->vpnWidget->setVpnSetting(m_vpnSetting);
    ui->vpnPpp->setVpnSetting(m_vpnSetting);
    ui->vpnIPsec->setVpnSetting(m_vpnSetting);
    ui->vpnIpvx->setIpv4Setting(m_ipv4Setting);
    ui->generalButton->setConnectionPtr(m_connection);
}

void VpnL2tpSetting::showSettingPage(QString activeConnectionPath)
{
    ui->generalWidget->showVpnSettings(VpnType::VPN_TYPE_L2TP);
    ui->vpnWidget->showSettings();
    ui->vpnPpp->showSettings();
    ui->vpnIPsec->showSettings();
    ui->vpnIpvx->showSeittngs();

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

void VpnL2tpSetting::saveSettingPage()
{
    ui->generalWidget->saveSettings();
    ui->vpnWidget->saveSettings();
    ui->vpnPpp->saveSettings();
    ui->vpnIPsec->saveSettings();
    ui->vpnIpvx->saveSettings();
}

void VpnL2tpSetting::clearPtr()
{
    m_vpnSetting.clear();
    SettingPage::clearPtr();

    ui->generalWidget->clearPtr();
    ui->vpnWidget->clearPtr();
    ui->vpnPpp->clearPtr();
    ui->vpnIPsec->clearPtr();
    ui->vpnIpvx->clearPtr();
    ui->generalButton->clearPtr();
}
