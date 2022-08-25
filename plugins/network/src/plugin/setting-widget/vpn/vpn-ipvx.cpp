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
#include "vpn-ipvx.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include "ui_vpn-ipvx.h"
using namespace NetworkManager;

Q_DECLARE_METATYPE(NetworkManager::Ipv4Setting::ConfigMethod)

VpnIpvx::VpnIpvx(QWidget *parent) : QWidget(parent), ui(new Ui::VpnIpvx)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

VpnIpvx::~VpnIpvx()
{
    delete ui;
}

void VpnIpvx::initUI()
{
    ui->ipv4Method->addItem(tr("Auto"), Ipv4Setting::ConfigMethod::Automatic);

    m_neverDefault = new KiranSwitchButton(this);
    m_neverDefault->setAccessibleName(QString("SwitchOnlyAppliedInCorrespondingResources"));
    ui->neverDefaultLayout->addWidget(m_neverDefault);
}

void VpnIpvx::initConnection()
{
}

void VpnIpvx::setIpv4Setting(const Ipv4Setting::Ptr &ipv4Setting)
{
    m_ipv4Setting = ipv4Setting;
}

void VpnIpvx::saveSettings()
{
    if (m_ipv4Setting != nullptr)
    {
        Ipv4Setting::ConfigMethod method = ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>();
        if (method == Ipv4Setting::ConfigMethod::Automatic)
        {
            m_ipv4Setting->setMethod(method);
        }
        else
        {
            //预留
        }

        m_ipv4Setting->setNeverDefault(m_neverDefault->isChecked());

        QList<QHostAddress> ipv4DNS;
        if (!ui->ipv4PreferredDNS->text().isEmpty())
        {
            ipv4DNS << QHostAddress(ui->ipv4PreferredDNS->text());
        }
        if (!ui->ipv4AlternateDNS->text().isEmpty())
        {
            ipv4DNS << QHostAddress(ui->ipv4AlternateDNS->text());
        }
        m_ipv4Setting->setDns(ipv4DNS);
    }
}

void VpnIpvx::showSeittngs()
{
    if (m_ipv4Setting != nullptr)
    {
        if (m_ipv4Setting->method() == Ipv4Setting::ConfigMethod::Automatic)
        {
            int ipv4MethodIndex = ui->ipv4Method->findData(Ipv4Setting::ConfigMethod::Automatic);
            ui->ipv4Method->setCurrentIndex(ipv4MethodIndex);
        }
        else
        {  //预留
        }
        m_neverDefault->setChecked(m_ipv4Setting->neverDefault());

        QString preferredDNS = "";
        QString alternateDNS = "";
        if (!m_ipv4Setting->dns().isEmpty())
        {
            preferredDNS = m_ipv4Setting->dns().at(0).toString();
            if (m_ipv4Setting->dns().count() >= 2)
            {
                alternateDNS = m_ipv4Setting->dns().at(1).toString();
            }
        }
        ui->ipv4PreferredDNS->setText(preferredDNS);
        ui->ipv4AlternateDNS->setText(alternateDNS);
    }
    else
        resetSettings();
}

void VpnIpvx::resetSettings()
{
    KLOG_DEBUG() << "VpnIpvx::resetSettings";
    int ipv4MethodIndex = ui->ipv4Method->findData(Ipv4Setting::ConfigMethod::Automatic);
    ui->ipv4Method->setCurrentIndex(ipv4MethodIndex);
    m_neverDefault->setChecked(false);
    ui->ipv4PreferredDNS->clear();
    ui->ipv4AlternateDNS->clear();
}

void VpnIpvx::clearPtr()
{
    m_ipv4Setting.clear();
}
