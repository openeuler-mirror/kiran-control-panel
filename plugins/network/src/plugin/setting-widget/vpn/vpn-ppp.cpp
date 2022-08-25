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

#include "vpn-ppp.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include "ui_vpn-ppp.h"
using namespace NetworkManager;

VpnPpp::VpnPpp(QWidget *parent) : QWidget(parent), ui(new Ui::VpnPpp)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

VpnPpp::~VpnPpp()
{
    delete ui;
}

void VpnPpp::initUI()
{
    m_useMPPE = new KiranSwitchButton(this);
    m_useMPPE->setAccessibleName(QString("SwitchUseMPPE"));
    m_statefulMPPE = new KiranSwitchButton(this);
    m_statefulMPPE->setAccessibleName(QString("SwitchStatefulMPPE"));

    ui->useMPPELayout->addWidget(m_useMPPE);
    ui->statefulMPPELayout->addWidget(m_statefulMPPE);

    m_useMPPE->setChecked(true);
    m_statefulMPPE->setChecked(false);
    ui->mppeSecurity->addItem(tr("All available (default)"), "require-mppe");
    ui->mppeSecurity->addItem(tr("40-bit (less secure)"), "require-mppe-40");
    ui->mppeSecurity->addItem(tr("128-bit (most secure)"), "require-mppe-128");

    QStringList supportOptions = {
        "refuse-eap", "refuse-pap", "refuse-chap", "refuse-mschap", "refuse-mschapv2",
        "nobsdcomp", "nodeflate", "no-vj-comp", "nopcomp", "noaccomp", "lcp-echo-interval"};
    initOptionsButton(supportOptions);
}

void VpnPpp::initOptionsButton(const QStringList &supportOptions)
{
    initOptionsStrMap();
    m_supportOptions = supportOptions;
    for (QString option : m_supportOptions)
    {
        const QString &str = m_optionsStrMap.key(option);
        if (!str.isEmpty())
        {
            QLabel *strLabel = new QLabel(str, this);
            KiranSwitchButton *switchButton = new KiranSwitchButton(this);
            switchButton->setProperty("option", option);
            switchButton->setAccessibleName(str);
            m_optionsButtonMap[option] = switchButton;

            QHBoxLayout *horizontalLayout = new QHBoxLayout();
            horizontalLayout->addWidget(strLabel);
            horizontalLayout->addStretch();
            horizontalLayout->addWidget(switchButton);
            horizontalLayout->setContentsMargins(0, 0, 0, 0);
            ui->optionsLayout->addLayout(horizontalLayout);
        }
        else
        {
            KLOG_DEBUG() << "Unsupport option:" << option;
        }
    }
}

void VpnPpp::initOptionsStrMap()
{
    /**
     * vpn.data：VPN 插件(例如NetworkManager-l2tp)特定数据的键/值对字典。键和值都必须为字符串。
     * "refuse-eap"等，为vpn.data的key
     * */
    m_optionsStrMap = {
        {tr("Refuse EAP Authentication"), "refuse-eap"},
        {tr("Refuse PAP Authentication"), "refuse-pap"},
        {tr("Refuse CHAP Authentication"), "refuse-chap"},
        {tr("Refuse MSCHAP Authentication"), "refuse-mschap"},
        {tr("Refuse MSCHAPv2 Authentication"), "refuse-mschapv2"},
        {tr("No BSD Data Compression"), "nobsdcomp"},
        {tr("No Deflate Data Compression"), "nodeflate"},
        {tr("No TCP Header Compression"), "no-vj-comp"},
        {tr("No Protocol Field Compression"), "nopcomp"},
        {tr("No Address/Control Compression"), "noaccomp"},
        {tr("Send PPP Echo Packets"), "lcp-echo-interval"}};
}

void VpnPpp::initConnection()
{
    connect(m_useMPPE, &QAbstractButton::toggled, this, &VpnPpp::handleMPPEChanged);
}

void VpnPpp::setVpnSetting(const VpnSetting::Ptr &vpnSetting)
{
    m_vpnSetting = vpnSetting;
}

void VpnPpp::saveSettings()
{
    m_dataMap = m_vpnSetting->data();
    QString mppeMethod = ui->mppeSecurity->currentData().toString();
    KLOG_DEBUG() << "mppeMethod:" << mppeMethod;
    if (m_useMPPE->isChecked())
    {
        m_dataMap.insert(mppeMethod, "yes");
    }
    else
    {
        m_dataMap.remove("require-mppe");
        m_dataMap.remove("require-mppe-40");
        m_dataMap.remove("require-mppe-128");
    }

    if (m_statefulMPPE->isChecked())
        m_dataMap.insert("mppe-stateful", "yes");
    else
        m_dataMap.remove("mppe-stateful");

    for (KiranSwitchButton *button : m_optionsButtonMap)
    {
        QString option = m_optionsButtonMap.key(button);
        KLOG_DEBUG() << "save option:" << option << "-----" << button->isChecked();
        if (button->isChecked())
        {
            if (option == "lcp-echo-interval")
            {
                m_dataMap.insert(option, "30");
                m_dataMap.insert("lcp-echo-failure", "5");
            }
            else
                m_dataMap.insert(option, "yes");
        }
        else
        {
            if (option == "lcp-echo-interval")
            {
                m_dataMap.remove("lcp-echo-failure");
            }
            m_dataMap.remove(option);
        }
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setInitialized(true);
}

// XXX:优化流程
void VpnPpp::showSettings()
{
    if (m_vpnSetting != nullptr)
    {
        m_useMPPE->setChecked(false);
        int index = ui->mppeSecurity->findData("require-mppe");
        ui->mppeSecurity->setCurrentIndex(index);
        ui->mppeWidget->setVisible(false);

        NMStringMap dataMap = m_vpnSetting->data();
        for (auto i = dataMap.cbegin(); i != dataMap.cend(); i++)
        {
            QString option = i.key();
            QString optionValue = i.value();
            KLOG_DEBUG() << "i.key():" << i.key();
            KLOG_DEBUG() << "i.value():" << i.value();

            QList<QString> requireMppeList;
            requireMppeList << "require-mppe"
                            << "require-mppe-40"
                            << "require-mppe-128";
            if (requireMppeList.contains(option))
            {
                if (optionValue == "yes")
                {
                    m_useMPPE->setChecked(true);
                    int index = ui->mppeSecurity->findData(option);
                    ui->mppeSecurity->setCurrentIndex(index);
                    ui->mppeWidget->setVisible(true);
                }
                else
                    ui->mppeWidget->setVisible(false);
            }
            else if (option == "mppe-stateful")
            {
                if (optionValue == "yes")
                    m_statefulMPPE->setChecked(true);
                else
                    m_statefulMPPE->setChecked(false);
            }
            else
            {
                auto button = m_optionsButtonMap.value(option);
                if (button != nullptr)
                {
                    if (option == "lcp-echo-interval")
                    {
                        if (!optionValue.isEmpty())
                            button->setChecked(true);
                        else
                            button->setChecked(false);
                    }
                    else
                    {
                        if (optionValue == "yes")
                            button->setChecked(true);
                        else
                            button->setChecked(false);
                    }
                }
            }
        }
    }
    else
        resetSettings();
}

void VpnPpp::resetSettings()
{
    m_useMPPE->setChecked(true);
    int index = ui->mppeSecurity->findData("require-mppe");
    ui->mppeSecurity->setCurrentIndex(index);
    m_statefulMPPE->setChecked(false);
    ui->mppeWidget->setVisible(true);
    for (auto button : m_optionsButtonMap)
    {
        button->setChecked(false);
    }
}

void VpnPpp::clearPtr()
{
    m_vpnSetting.clear();
}

void VpnPpp::handleMPPEChanged(bool checked)
{
    //    if (!checked)
    //    {
    //        ui->mppeSecurity->setCurrentIndex(0);
    //        m_statefulMPPE->setChecked(false);
    //    }
    ui->mppeWidget->setVisible(checked);
}
