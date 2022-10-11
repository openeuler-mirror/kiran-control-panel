/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#include "tray-page.h"
#include <qt5-log-i.h>
#include "ui_tray-page.h"
#include "wired-tray-widget.h"
#include "wireless-tray-widget.h"
using namespace NetworkManager;

TrayPage::TrayPage(Device::List deviceList, QWidget *parent) : QWidget(parent), ui(new Ui::TrayPage)
{
    ui->setupUi(this);
    m_deviceList = deviceList;
    init();
}

TrayPage::~TrayPage()
{
    delete ui;
}

void TrayPage::init()
{
    if (m_deviceList.count() != 0)
    {
        initUI();
        initConnection();
    }
}

// XXX:修改初始化
void TrayPage::initUI()
{
    setFixedWidth(240);
    setMaximumHeight(434);
    ui->stackedWidget->setContentsMargins(0, 0, 0, 0);

    // if (m_deviceList.count() > 1)
    //     setMultiDeviceWidget();
    // else if (m_deviceList.count() == 1)
    // {
    //     setSingleDeviceWidget();
    // }
    // else
    // {
    //     // m_deviceeList.count == 0
    //     return;
    // }

    if (m_deviceList.count() != 0)
    {
        setMultiDeviceWidget();
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

void TrayPage::setMultiDeviceWidget()
{
    for (Device::Ptr dev : m_deviceList)
    {
        QString devicePath = dev->uni();
        QString deviceName = dev->interfaceName();
        Device::Type deviceType = dev->type();
        ui->deviceComboBox->addItem(deviceName, devicePath);

        if (deviceType == Device::Ethernet)
        {
            ui->deviceLabel->setText(tr("Select wired network card"));

            QWidget *widget = new QWidget();
            auto vLayout = new QVBoxLayout(widget);
            vLayout->setSpacing(0);
            vLayout->setContentsMargins(0, 0, 0, 0);

            WiredTrayWidget *wiredTrayWidget = new WiredTrayWidget(devicePath, this);
            vLayout->addWidget(wiredTrayWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
            ui->stackedWidget->addWidget(widget);

            connect(wiredTrayWidget, &WiredTrayWidget::adjustedTraySize, this, &TrayPage::adjustedTraySize);
        }
        else if (deviceType == Device::Wifi)
        {
            ui->deviceLabel->setText(tr("Select wireless network card"));
            QWidget *widget = new QWidget();
            auto vLayout = new QVBoxLayout(widget);
            vLayout->setSpacing(0);
            vLayout->setContentsMargins(0, 0, 0, 0);

            WirelessTrayWidget *wirelessTrayWidget = new WirelessTrayWidget(devicePath, this);
            vLayout->addWidget(wirelessTrayWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
            ui->stackedWidget->addWidget(widget);

            connect(wirelessTrayWidget, &WirelessTrayWidget::adjustedTraySize, this, &TrayPage::adjustedTraySize);
        }
    }
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TrayPage::handleDeviceComboBoxChanged);
    ui->selectDevicewidget->setVisible(true);
}

void TrayPage::setSingleDeviceWidget()
{
    ui->selectDevicewidget->setVisible(false);
    QString devicePath = m_deviceList.value(0)->uni();
    Device::Type deviceType = m_deviceList.value(0)->type();
    if (deviceType == Device::Ethernet)
    {
        WiredTrayWidget *wiredTrayWidget = new WiredTrayWidget(devicePath, this);
        ui->stackedWidget->addWidget(wiredTrayWidget);
        connect(wiredTrayWidget, &WiredTrayWidget::adjustedTraySize, this, &TrayPage::adjustedTraySize);
    }
    else if (deviceType == Device::Wifi)
    {
        WirelessTrayWidget *wirelessTrayWidget = new WirelessTrayWidget(devicePath, this);
        ui->stackedWidget->addWidget(wirelessTrayWidget);
        connect(wirelessTrayWidget, &WirelessTrayWidget::adjustedTraySize, this, &TrayPage::adjustedTraySize);
    }
}

void TrayPage::handleDeviceComboBoxChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

QStringList TrayPage::devicePathList()
{
    QStringList devicePathList;
    for (Device::Ptr device : m_deviceList)
    {
        if (device != nullptr)
            devicePathList << device->uni();
    }
    KLOG_DEBUG() << "devicePathList:" << devicePathList;
    return devicePathList;
}
