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

#include "tray-page.h"
#include <qt5-log-i.h>
#include "ui_tray-page.h"
#include "wired-tray-widget.h"
#include "wireless-tray-widget.h"

TrayPage::TrayPage(Device::List deviceList, QWidget *parent) : QWidget(parent), ui(new Ui::TrayPage)
{
    ui->setupUi(this);
    m_deviceList = deviceList;
    init();
//    setStyleSheet("background:bule;");
}

TrayPage::~TrayPage()
{
    delete ui;
}

void TrayPage::init()
{
    Device::Type deviceType = m_deviceList.at(0)->type();
    initUI(deviceType);
    initConnection();
}

void TrayPage::initUI(Device::Type deviceType)
{
    setFixedWidth(240);
    setMaximumHeight(434);
    ui->stackedWidget->setContentsMargins(0, 0, 0, 0);

    if (m_deviceList.count() > 1)
        setMultiDeviceWidget(deviceType);
    else if (m_deviceList.count() == 1)
    {
        ui->selectDevicewidget->setVisible(false);
        QString devicePath = m_deviceList.at(0)->uni();
        if (deviceType == Device::Ethernet)
        {
            WiredTrayWidget *wiredTrayWidget = new WiredTrayWidget(devicePath, this);
            ui->stackedWidget->addWidget(wiredTrayWidget);
            connect(wiredTrayWidget,&WiredTrayWidget::adjustedTraySize,this,&TrayPage::adjustedTraySize);
        }
        else if (deviceType == Device::Wifi)
        {
            WirelessTrayWidget *wirelessTrayWidget = new WirelessTrayWidget(devicePath, this);
            ui->stackedWidget->addWidget(wirelessTrayWidget);
            connect(wirelessTrayWidget, &WirelessTrayWidget::adjustedTraySize,this,&TrayPage::adjustedTraySize);
        }
    }
    else
    {
        // m_deviceeList.count == 0
        return;
    }
}

void TrayPage::initConnection()
{

}

void TrayPage::setMultiDeviceWidget(Device::Type deviceType)
{
    for (Device::Ptr dev : m_deviceList)
    {
        QString devicePath = dev->uni();
        QString deviceName = dev->interfaceName();
        ui->deviceComboBox->addItem(deviceName, devicePath);

        if (deviceType == Device::Ethernet)
        {
            ui->deviceLabel->setText(tr("Select wired network card"));

            QWidget *widget = new QWidget();
            auto vLayout = new QVBoxLayout(widget);
            vLayout->setSpacing(0);
            vLayout->setContentsMargins(0, 0, 0, 0);

            WiredTrayWidget *wiredTrayWidget = new WiredTrayWidget(devicePath, this);
            vLayout->addWidget(wiredTrayWidget,0,Qt::AlignHCenter|Qt::AlignTop);
            ui->stackedWidget->addWidget(widget);

            connect(wiredTrayWidget, &WiredTrayWidget::adjustedTraySize,this,&TrayPage::adjustedTraySize);
        }
        else if (deviceType == Device::Wifi)
        {
            ui->deviceLabel->setText(tr("Select wireless network card"));
            QWidget *widget = new QWidget();
            auto vLayout = new QVBoxLayout(widget);
            vLayout->setSpacing(0);
            vLayout->setContentsMargins(0, 0, 0, 0);

            WirelessTrayWidget *wirelessTrayWidget = new WirelessTrayWidget(devicePath, this);
            vLayout->addWidget(wirelessTrayWidget,0,Qt::AlignHCenter|Qt::AlignTop);
            ui->stackedWidget->addWidget(widget);

            connect(wirelessTrayWidget, &WirelessTrayWidget::adjustedTraySize,this,&TrayPage::adjustedTraySize);
        }
    }
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TrayPage::handleDeviceComboBoxChanged);
    ui->selectDevicewidget->setVisible(true);
}

void TrayPage::handleDeviceComboBoxChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

QStringList TrayPage::devicePathList()
{
    QStringList devicePathList;
    KLOG_DEBUG() << "m_deviceList:" << m_deviceList;
    for (Device::Ptr device : m_deviceList)
    {
        if (device == nullptr)
            KLOG_DEBUG() << "device == nullptr";
        else
            devicePathList << device->uni();
    }
    KLOG_DEBUG() << "devicePathList:" << devicePathList;
    return devicePathList;
}


