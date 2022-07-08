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

#include "wireless-widget.h"
#include "ui_wireless-widget.h"
#include "kiran-switch-button.h"
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <qt5-log-i.h>
WirelessWidget::WirelessWidget(QWidget *parent) : QWidget(parent), ui(new Ui::WirelessWidget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

WirelessWidget::~WirelessWidget()
{
    delete ui;
}

void WirelessWidget::initUI()
{
    ui->ssidEdit->setPlaceholderText(tr("Required"));
    ui->ssidEdit->setEnabled(false);
    m_mtuButton = new KiranSwitchButton(this);
    ui->mtuLayout->addWidget(m_mtuButton);
    ui->customMTU->setVisible(false);
    ui->customMTU->setMinimum(0);
    ui->customMTU->setMaximum(10000);
    //UserData设为空""，为了匹配不指定设备的情况
    ui->deviceMac->addItem(tr("No device specified"), "");
    initMacComboBox();
}

void WirelessWidget::initConnection()
{
    connect(m_mtuButton, &QAbstractButton::toggled, this, &WirelessWidget::handleCustomMTUChanged);
    connect(notifier(),&Notifier::deviceAdded,[=](){initMacComboBox();});
    connect(notifier(),&Notifier::deviceRemoved,[=](){initMacComboBox();});
}

void WirelessWidget::setWirelessSetting(const WirelessSetting::Ptr &wirelessSetting)
{
    m_wirelessSetting = wirelessSetting;
}

//区分连接隐藏网络和已存在网络的Setting
void WirelessWidget::saveSettings()
{
    if(m_wirelessSetting != nullptr)
    {
        QString macAddress = ui->deviceMac->currentData().toString();
        KLOG_DEBUG() << "macAddress:" << macAddress;
        m_wirelessSetting->setMacAddress(QByteArray::fromHex(macAddress.toUtf8()));
        m_wirelessSetting->setMtu(ui->customMTU->value());
    }
}

void WirelessWidget::showSettings()
{
    if(m_wirelessSetting != nullptr)
    {
        QString deviceMac = m_wirelessSetting->macAddress().toHex(':').toUpper();
        quint32 mtu = m_wirelessSetting->mtu();
        int deviceMacIndex = ui->deviceMac->findData(deviceMac);
        ui->deviceMac->setCurrentIndex(deviceMacIndex);

        if (mtu != 0)
            ui->customMTU->setVisible(true);
        else
            ui->customMTU->setVisible(false);
        ui->customMTU->setValue(mtu);
        ui->ssidEdit->setText(m_wirelessSetting->ssid());
    }
    else
        resetSettings();
}

void WirelessWidget::setOtherWirelessSetting()
{

}

void WirelessWidget::resetSettings()
{
    ui->ssidEdit->clear();
    int deviceMacIndex = ui->deviceMac->findData("");
    ui->deviceMac->setCurrentIndex(deviceMacIndex);
    ui->customMTU->setVisible(false);
}


void WirelessWidget::clearPtr()
{
    m_wirelessSetting.clear();
}

void WirelessWidget::handleCustomMTUChanged(bool checked)
{
    if (!checked)
    {
        ui->customMTU->setValue(0);
    }
    ui->customMTU->setVisible(checked);
}


void WirelessWidget::initMacComboBox()
{
    const Device::List deviceList = networkInterfaces();
    WirelessDevice::Ptr wirelessDevice;
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == Device::Wifi)
        {
            wirelessDevice = qobject_cast<WirelessDevice *>(dev);
            QString macAddress = wirelessDevice->permanentHardwareAddress();
            if (macAddress.isEmpty())
            {
                macAddress = wirelessDevice->hardwareAddress();
            }
            QString macStr = macAddress + "(" + wirelessDevice->interfaceName() + ")";
            ui->deviceMac->addItem(macStr, macAddress);
        }
    }
}

