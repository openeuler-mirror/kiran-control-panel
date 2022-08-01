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

#include "vpn-ipsec.h"
#include "ui_vpn-ipsec.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
using namespace NetworkManager;

VpnIPsec::VpnIPsec(QWidget *parent) : QWidget(parent), ui(new Ui::VpnIPsec)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

VpnIPsec::~VpnIPsec()
{
    delete ui;
}

void VpnIPsec::initUI()
{
    m_enableIPsec = new KiranSwitchButton(this);
    ui->enableIPsecLayout->addWidget(m_enableIPsec);
    m_enableIPsec->setChecked(false);
    ui->IPsecWidget->setVisible(false);
}

void VpnIPsec::initConnection()
{
    connect(m_enableIPsec,&QAbstractButton::toggled,[=](bool checked){
        ui->IPsecWidget->setVisible(checked);
    });
}

void VpnIPsec::setVpnSetting(const VpnSetting::Ptr &vpnSetting)
{
    m_vpnSetting = vpnSetting;
}

void VpnIPsec::saveSettings()
{
    m_dataMap = m_vpnSetting->data();

    if (m_enableIPsec->isChecked()) {
        m_dataMap.insert("ipsec-enabled", "yes");
        m_dataMap.insert("ipsec-group-name", ui->groupName->text());
        m_dataMap.insert("ipsec-gateway-id", ui->groupId->text());
        m_dataMap.insert("ipsec-psk", ui->preSharedKey->text());
        m_dataMap.insert("ipsec-ike", ui->ipsecIKE->text());
        m_dataMap.insert("ipsec-esp", ui->ipsecESP->text());
    } else {
        m_dataMap.remove("ipsec-enabled");
        m_dataMap.remove("ipsec-group-name");
        m_dataMap.remove("ipsec-gateway-id");
        m_dataMap.remove("ipsec-psk");
        m_dataMap.remove("ipsec-ike");
        m_dataMap.remove("ipsec-esp");
    }

    KLOG_DEBUG() << "m_dataMap:" << m_dataMap;
    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setInitialized(true);
}

void VpnIPsec::showSettings()
{
    if(m_vpnSetting != nullptr)
    {
        NMStringMap dataMap = m_vpnSetting->data();
        m_enableIPsec->setChecked(dataMap.value("ipsec-enabled") == "yes");

        ui->groupName->setText(dataMap.value("ipsec-group-name"));
        ui->groupId->setText(dataMap.value("ipsec-gateway-id"));
        ui->preSharedKey->setText(dataMap.value("ipsec-psk"));
        ui->ipsecIKE->setText(dataMap.value("ipsec-ike"));
        ui->ipsecESP->setText(dataMap.value("ipsec-esp"));
    }
    else
        resetSettings();
}

void VpnIPsec::resetSettings()
{
    m_enableIPsec->setChecked(false);
    ui->IPsecWidget->setVisible(false);
    ui->groupName->clear();
    ui->groupId->clear();
    ui->preSharedKey->clear();
    ui->ipsecIKE->clear();
    ui->ipsecESP->clear();
}

void VpnIPsec::clearPtr()
{
    m_vpnSetting.clear();
}
