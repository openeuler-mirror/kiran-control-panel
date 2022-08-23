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
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include "ui_vpn-l2tp-setting.h"
#define ServiceTypeL2TP "org.freedesktop.NetworkManager.l2tp"
using namespace NetworkManager;

VpnL2tpSetting::VpnL2tpSetting(QWidget *parent) : SettingPage(parent), ui(new Ui::VpnL2tpSetting)
{
    ui->setupUi(this);
    ui->connectioNameWidget->setNameLabel(tr("VPN name"));
    initConnection();
}

VpnL2tpSetting::~VpnL2tpSetting()
{
    delete ui;
}

void VpnL2tpSetting::initConnection()
{
    connect(ui->disconnectAndDeleteWidget, &DisconnectAndDeleteButton::disconnectButtonClicked, this, &VpnL2tpSetting::returnPreviousPage);
    connect(ui->disconnectAndDeleteWidget, &DisconnectAndDeleteButton::deleteButtonClicked, this, &VpnL2tpSetting::returnPreviousPage);
}

void VpnL2tpSetting::initSettingPage()
{
    initSpecificSettings();
}

void VpnL2tpSetting::initSpecificSettings()
{
    m_vpnSetting = m_connectionSettings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
    m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();

    m_vpnSetting->setServiceType(ServiceTypeL2TP);
    m_vpnSetting->setInitialized(true);
    m_ipv4Setting->setInitialized(true);

    ui->connectioNameWidget->setConnectionSettings(m_connectionSettings);
    ui->vpnWidget->setVpnSetting(m_vpnSetting);
    ui->vpnPpp->setVpnSetting(m_vpnSetting);
    ui->vpnIPsec->setVpnSetting(m_vpnSetting);
    ui->vpnIpvx->setIpv4Setting(m_ipv4Setting);
    ui->disconnectAndDeleteWidget->setConnectionPtr(m_connection);
}

void VpnL2tpSetting::showSettingPage(QString activeConnectionPath)
{
    ui->connectioNameWidget->showVpnSettings(VpnType::VPN_TYPE_L2TP);
    ui->vpnWidget->showSettings();
    ui->vpnPpp->showSettings();
    ui->vpnIPsec->showSettings();
    ui->vpnIpvx->showSeittngs();

    if (m_connectionSettings.isNull())
    {
        ui->disconnectAndDeleteWidget->initButton(SETTING_CONNECTION_STATUS_NEW);
        ui->disconnectAndDeleteWidget->setVisible(false);
    }
    else
    {
        //通过将激活路径传入SettingPage,判断该连接是否激活，也可通过uuid判断
        ui->disconnectAndDeleteWidget->setVisible(true);
        if (activeConnectionPath.isEmpty())
            ui->disconnectAndDeleteWidget->initButton(SETTING_CONNECTION_STATUS_DEACTIVATED);
        else
            ui->disconnectAndDeleteWidget->initButton(SETTING_CONNECTION_STATUS_ACTIVATED, activeConnectionPath);
    }
}

void VpnL2tpSetting::saveSettingPage()
{
    ui->connectioNameWidget->saveSettings();
    ui->vpnWidget->saveSettings();
    ui->vpnPpp->saveSettings();
    ui->vpnIPsec->saveSettings();
    ui->vpnIpvx->saveSettings();
}

void VpnL2tpSetting::clearPtr()
{
    m_vpnSetting.clear();
    SettingPage::clearPtr();

    ui->connectioNameWidget->clearPtr();
    ui->vpnWidget->clearPtr();
    ui->vpnPpp->clearPtr();
    ui->vpnIPsec->clearPtr();
    ui->vpnIpvx->clearPtr();
    ui->disconnectAndDeleteWidget->clearPtr();
}

bool VpnL2tpSetting::isInputValid()
{
    return ui->vpnWidget->isInputValid() && ui->connectioNameWidget->isInputValid();
}
