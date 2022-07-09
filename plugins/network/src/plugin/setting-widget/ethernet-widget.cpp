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

#include "ethernet-widget.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include "ui_ethernet-widget.h"
EthernetWidget::EthernetWidget(QWidget *parent) : QWidget(parent), ui(new Ui::EthernetWidget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

EthernetWidget::~EthernetWidget()
{
    delete ui;
}
void EthernetWidget::initUI()
{
    m_mtuButton = new KiranSwitchButton(this);
    ui->mtuLayout->addWidget(m_mtuButton);
    ui->customMTU->setVisible(false);
    ui->customMTU->setMinimum(0);
    ui->customMTU->setMaximum(10000);
    //UserData设为空""，为了匹配Mac地址为空的情况
    ui->deviceMac->addItem(tr("No device specified"), "");
    initEthernetMacComboBox();
}

void EthernetWidget::initEthernetMacComboBox()
{
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

void EthernetWidget::initConnection()
{
    connect(m_mtuButton, &QAbstractButton::toggled, this, &EthernetWidget::handleCustomMTUChanged);
}

void EthernetWidget::setWiredSetting(const WiredSetting::Ptr &wiredSetting)
{
    m_wiredSetting = wiredSetting;
}

void EthernetWidget::saveSettings()
{
    if(m_wiredSetting != nullptr)
    {
        QString macAddress = ui->deviceMac->currentData().toString();
        QString cloneMac = ui->cloneDeviceMac->text();
        KLOG_DEBUG() << "macAddress:" << macAddress;
        KLOG_DEBUG() << "cloneMac:" << cloneMac;
        m_wiredSetting->setMacAddress(QByteArray::fromHex(macAddress.toUtf8()));
        m_wiredSetting->setClonedMacAddress(QByteArray::fromHex(cloneMac.toUtf8()));
        m_wiredSetting->setMtu(ui->customMTU->value());
    }
}

void EthernetWidget::showSettings()
{
    if(m_wiredSetting != nullptr)
    {
        QString deviceMac = m_wiredSetting->macAddress().toHex(':').toUpper();
        QString cloneDeviceMac = m_wiredSetting->clonedMacAddress().toHex(':').toUpper();
        quint32 mtu = m_wiredSetting->mtu();

        int deviceMacIndex = ui->deviceMac->findData(deviceMac);
        ui->deviceMac->setCurrentIndex(deviceMacIndex);
        ui->cloneDeviceMac->setText(cloneDeviceMac);

        if (mtu != 0)
            ui->customMTU->setVisible(true);
        else
            ui->customMTU->setVisible(false);
        ui->customMTU->setValue(mtu);
    }
    else
        resetSettings();
}

void EthernetWidget::handleCustomMTUChanged(bool checked)
{
    if (!checked)
    {
        ui->customMTU->setValue(0);
    }
    ui->customMTU->setVisible(checked);
}
void EthernetWidget::resetSettings()
{
    int deviceMacIndex = ui->deviceMac->findData("");
    ui->deviceMac->setCurrentIndex(deviceMacIndex);
    ui->cloneDeviceMac->clear();
    ui->customMTU->setVisible(false);
}

void EthernetWidget::clearPtr()
{
    m_wiredSetting.clear();
}

bool EthernetWidget::isInputValid()
{
    isCloneMacValid(ui->cloneDeviceMac->text());
    return false;
}

bool EthernetWidget::isCloneMacValid(const QString &cloneMac)
{
    if (cloneMac.isEmpty()) {
        return true;
    }
    bool matched =  QRegExp("^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$").exactMatch(cloneMac);
    return matched;
}
