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

#include "vpn-pptp-setting.h"
#include "ui_vpn-pptp-setting.h"
#include <qt5-log-i.h>
#define ServiceTypePPTP "org.freedesktop.NetworkManager.pptp"
VpnPptpSetting::VpnPptpSetting(QWidget *parent) : SettingPage(parent), ui(new Ui::VpnPptpSetting)
{
    ui->setupUi(this);
    ui->generalWidget->setNameLabel(tr("VPN name"));
    connect(ui->generalButton,&GeneralButton::disconnectButtonClicked,this,&VpnPptpSetting::returnPreviousPage);
    connect(ui->generalButton,&GeneralButton::deleteButtonClicked,this,&VpnPptpSetting::returnPreviousPage);
}

VpnPptpSetting::~VpnPptpSetting()
{
    delete ui;
}

void VpnPptpSetting::initSettingPage()
{
    initSpecificSettings();
    initWidgets();
}


void VpnPptpSetting::initSpecificSettings()
{
    m_vpnSetting = m_connectionSettings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
    m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
    m_vpnSetting->setServiceType(ServiceTypePPTP);
}

void VpnPptpSetting::initWidgets()
{
    ui->generalWidget->setConnectionSettings(m_connectionSettings);
    ui->vpnWidget->setVpnSetting(m_vpnSetting);
    ui->vpnPpp->setVpnSetting(m_vpnSetting);
    ui->vpnIpvx->setIpv4Setting(m_ipv4Setting);
    ui->generalButton->setConnectionPtr(m_connection);
}

void VpnPptpSetting::saveSettingPage()
{
    ui->generalWidget->saveSettings();
    ui->vpnWidget->saveSettings();
    ui->vpnPpp->saveSettings();
    ui->vpnIpvx->saveSettings();
}

void VpnPptpSetting::showSettingPage(QString activeConnectionPath)
{
    ui->generalWidget->showVpnSettings(VpnType::VPN_TYPE_PPTP);
    ui->vpnWidget->showSettings();
    ui->vpnPpp->showSettings();
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

void VpnPptpSetting::clearPtr()
{
    KLOG_DEBUG() << "VpnPptpSetting::clearPtr" ;
    m_vpnSetting.clear();
    SettingPage::clearPtr();

    ui->generalWidget->clearPtr();
    ui->vpnWidget->clearPtr();
    ui->vpnPpp->clearPtr();
    ui->vpnIpvx->clearPtr();
    ui->generalButton->clearPtr();
}
