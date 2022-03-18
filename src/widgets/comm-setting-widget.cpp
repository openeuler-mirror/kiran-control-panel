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

#include "comm-setting-widget.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include "ui_comm-setting-widget.h"

using namespace NetworkManager;
CommSettingWidget::CommSettingWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CommSettingWidget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

CommSettingWidget::~CommSettingWidget()
{
    delete ui;
}

void CommSettingWidget::initUI()
{
    ui->ipv4Method->addItem(tr("Auto"), Ipv4Setting::ConfigMethod::Automatic);
    ui->ipv4Method->addItem(tr("Manual"), Ipv4Setting::ConfigMethod::Manual);
    ui->ipv4Manual->setVisible(false);

    ui->ipv6Method->addItem(tr("Auto"), Ipv6Setting::ConfigMethod::Automatic);
    ui->ipv6Method->addItem(tr("Manual"), Ipv6Setting::ConfigMethod::Manual);
    ui->ipv6Method->addItem(tr("Ignored"), Ipv6Setting::ConfigMethod::Ignored);
    ui->ipv6Manual->setVisible(false);

    m_mtuButton = new KiranSwitchButton(this);
    ui->mtuLayout->addWidget(m_mtuButton);
    ui->customMTU->setVisible(false);
}

void CommSettingWidget::initConnection()
{
    connect(ui->ipv4Method, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        handleIpv4MethodChanged(ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>());
    });

    connect(ui->ipv6Method, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        handleIpv6MethodChanged(ui->ipv6Method->currentData().value<NetworkManager::Ipv6Setting::ConfigMethod>());
    });

    connect(ui->deviceMac, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        KLOG_DEBUG() << "ui->deviceMac->currentData():" << ui->deviceMac->currentData().toString();
    });

    connect(ui->disconnectButton, &QPushButton::clicked, [=]() {
        emit disconnect();
    });

    connect(ui->deleteButton, &QPushButton::clicked, [=]() {
        emit deleteConnetion();
    });

    connect(m_mtuButton, &QAbstractButton::toggled, this, &CommSettingWidget::handleCustomMTUChanged);
}

void CommSettingWidget::handleIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method)
{
    switch (method)
    {
    case Ipv4Setting::ConfigMethod::Automatic:
        ui->ipv4Manual->setVisible(false);
        KLOG_DEBUG() << "Automatic";
        break;
    case Ipv4Setting::ConfigMethod::Manual:
        ui->ipv4Manual->setVisible(true);
        KLOG_DEBUG() << "Manual";
        break;
    default:
        break;
    }
}

void CommSettingWidget::handleIpv6MethodChanged(NetworkManager::Ipv6Setting::ConfigMethod method)
{
    switch (method)
    {
    case Ipv6Setting::ConfigMethod::Automatic:
        ui->ipv6Manual->setVisible(false);
        KLOG_DEBUG() << "Automatic";
        break;
    case Ipv6Setting::ConfigMethod::Manual:
        ui->ipv6Manual->setVisible(true);
        KLOG_DEBUG() << "Manual";
        break;
    case Ipv6Setting::ConfigMethod::Ignored:
        ui->ipv6Manual->setVisible(false);
        KLOG_DEBUG() << "Ignored";
        break;
    default:
        break;
    }
}

CommConfigInfo CommSettingWidget::getCommConfig()
{
    m_configInfo.ipv4Method = ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>();
    m_configInfo.ipv4Address = ui->ipv4Address->text();
    m_configInfo.ipv4Netmask = ui->ipv4Netmask->text();
    m_configInfo.ipv4Gateway = ui->ipv4Gateway->text();
    m_configInfo.ipv4PreferredDNS = ui->ipv4PreferredDNS->text();
    m_configInfo.ipv4AlternateDNS = ui->ipv4AlternateDNS->text();

    m_configInfo.ipv6Method = ui->ipv6Method->currentData().value<NetworkManager::Ipv6Setting::ConfigMethod>();
    m_configInfo.ipv6Address = ui->ipv6Address->text();
    m_configInfo.ipv6Prefix = ui->ipv6Prefix->value();

    m_configInfo.ipv6Gateway = ui->ipv6Gateway->text();
    m_configInfo.ipv6PreferredDNS = ui->ipv6PreferredDNS->text();
    m_configInfo.ipv6AlternateDNS = ui->ipv6AlternateDNS->text();

    m_configInfo.ethernetDeviceMac = ui->deviceMac->currentData().toString();
    m_configInfo.ethernetCloneDeviceMac = ui->cloneDeviceMac->text();

    m_configInfo.mtu = ui->customMTU->value();

    return m_configInfo;
}

void CommSettingWidget::initCommConfig(CommConfigInfo configInfo)
{
    int ipv4MethodIndex = ui->ipv4Method->findData(configInfo.ipv4Method);
    ui->ipv4Method->setCurrentIndex(ipv4MethodIndex);
    ui->ipv4Address->setText(configInfo.ipv4Address);
    ui->ipv4Netmask->setText(configInfo.ipv4Netmask);
    ui->ipv4Gateway->setText(configInfo.ipv4Gateway);
    ui->ipv4PreferredDNS->setText(configInfo.ipv4PreferredDNS);
    ui->ipv4AlternateDNS->setText(configInfo.ipv4AlternateDNS);

    int ipv6MethodIndex = ui->ipv6Method->findData(configInfo.ipv6Method);
    ui->ipv6Method->setCurrentIndex(ipv6MethodIndex);
    ui->ipv6Address->setText(configInfo.ipv6Address);
    ui->ipv6Prefix->setValue(configInfo.ipv6Prefix);

    ui->ipv6Gateway->setText(configInfo.ipv6Gateway);
    ui->ipv6PreferredDNS->setText(configInfo.ipv6PreferredDNS);
    ui->ipv6AlternateDNS->setText(configInfo.ipv6AlternateDNS);

    initEthernetMacComboBox();
    KLOG_DEBUG() << "configInfo.ethernetDeviceMac:" << configInfo.ethernetDeviceMac;
    int deviceMacIndex = ui->deviceMac->findData(configInfo.ethernetDeviceMac);
    ui->deviceMac->setCurrentIndex(deviceMacIndex);
    ui->cloneDeviceMac->setText(configInfo.ethernetCloneDeviceMac);

    if (configInfo.mtu != 0)
        ui->customMTU->setVisible(true);
    else
        ui->customMTU->setVisible(false);
    ui->customMTU->setValue(configInfo.mtu);
}

void CommSettingWidget::initDisconnectAndDeleteButton(SettingConnectionStatus connectionStatus)
{
    switch (connectionStatus)
    {
    case SETTING_CONNECTION_STATUS_NEW:
        ui->deleteButton->setVisible(false);
        ui->disconnectButton->setVisible(false);
        break;
    case SETTING_CONNECTION_STATUS_DEACTIVATED:
        ui->deleteButton->setVisible(true);
        ui->disconnectButton->setVisible(false);
        break;
    case SETTING_CONNECTION_STATUS_ACTIVATED:
        ui->deleteButton->setVisible(true);
        ui->disconnectButton->setVisible(true);
        break;
    default:
        break;
    }
}

void CommSettingWidget::handleCustomMTUChanged(bool checked)
{
    if (!checked)
    {
        ui->customMTU->setValue(0);
    }
    ui->customMTU->setVisible(checked);
}

void CommSettingWidget::initEthernetMacComboBox()
{
    ui->deviceMac->addItem(tr("No device specified"), "");  //UserData设为空""，为了匹配Mac地址为空的情况

    const Device::List deviceList = networkInterfaces();
    WiredDevice::Ptr wiredDevice;
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == Device::Ethernet)
        {
            wiredDevice = qobject_cast<WiredDevice *>(dev);
            QString macAddress = wiredDevice->permanentHardwareAddress();
            if (macAddress.isEmpty())
            {
                macAddress = wiredDevice->hardwareAddress();
            }
            QString macStr = macAddress + "(" + wiredDevice->interfaceName() + ")";
            ui->deviceMac->addItem(macStr, macAddress);
        }
    }
}

void CommSettingWidget::setIpv6WidgetVisible(bool visible)
{
    ui->ipv6Widget->setVisible(visible);
}
