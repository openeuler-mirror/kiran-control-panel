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

#include "cpanel-network-widget.h"
#include <qt5-log-i.h>
#include "plugin/details-page.h"
#include "ui_cpanel-network-widget.h"
#include "vpn-manager.h"
#include "wired-manager.h"
#include "wireless-manager.h"

enum NetworkSettingsPages
{
    PAGE_WIRED,
    PAGE_WIRELESS,
    PAGE_VPN
};

CPanelNetworkWidget::CPanelNetworkWidget(QWidget* parent) : QWidget(parent), ui(new Ui::CPanelNetworkWidget)
{
    ui->setupUi(this);
    init();
}

CPanelNetworkWidget::~CPanelNetworkWidget()
{
    delete ui;
}

void CPanelNetworkWidget::init()
{
    initPage();

//    ui->sidebar->insertItem(PAGE_WIRED, tr("Wired Network"));
//    ui->sidebar->insertItem(PAGE_WIRELESS, tr("Wireless"));
//    ui->sidebar->insertItem(PAGE_VPN, tr("VPN"));
//
//    ui->sidebar->item(PAGE_WIRED)->setData(Qt::UserRole, PAGE_WIRED);
//    ui->sidebar->item(PAGE_WIRELESS)->setData(Qt::UserRole, PAGE_WIRELESS);
//    ui->sidebar->item(PAGE_VPN)->setData(Qt::UserRole, PAGE_VPN);

    connect(ui->sidebar, &QListWidget::currentItemChanged, [this](QListWidgetItem* current, QListWidgetItem* previous) {
                ui->stackedWidget->setCurrentIndex(current->data(Qt::UserRole).toInt());
            });
}

void CPanelNetworkWidget::initPage()
{
    getAvailableDeviceList();
    int row = 0;
    for (Device::Ptr device : m_wiredDeviceList)
    {
        QString devicePath = device->uni();
        QString deviceName = device->interfaceName();
        WiredManager *wiredManager = new WiredManager(devicePath,this);
        ui->stackedWidget->addWidget(wiredManager);
        ui->sidebar->insertItem(row,deviceName);
        ui->sidebar->item(row)->setData(Qt::UserRole, row);
        row++;
    }
    for (Device::Ptr device : m_wirelessDeviceList)
    {
        QString devicePath = device->uni();
        QString deviceName = device->interfaceName();
        WirelessManager *wirelessManager = new WirelessManager(devicePath,this);
        ui->stackedWidget->addWidget(wirelessManager);
        ui->sidebar->insertItem(row,deviceName);
        ui->sidebar->item(row)->setData(Qt::UserRole, row);
        row++;
    }
    VpnManager *vpnManager = new VpnManager(this);
    ui->stackedWidget->addWidget(vpnManager);
    ui->sidebar->insertItem(row,"VPN");
    ui->sidebar->item(row)->setData(Qt::UserRole, row);
    row++;

    DetailsPage *networkDetails = new DetailsPage(this);
    ui->stackedWidget->addWidget(networkDetails);
    ui->sidebar->insertItem(row,"Network Details");
    ui->sidebar->item(row)->setData(Qt::UserRole, row);
    row++;

}

void CPanelNetworkWidget::getAvailableDeviceList()
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        KLOG_DEBUG() << "dev->interfaceName():" << dev->interfaceName();
        KLOG_DEBUG() << "dev->managed():" << dev->managed();
        KLOG_DEBUG() << "dev->availableConnections():" << dev->availableConnections();
        KLOG_DEBUG() << "dev->state():" << dev->state();

        if(dev->state() == Device::Unavailable)
            continue ;

        switch (dev->type())
        {
        case Device::Ethernet:
            m_wiredDeviceList << dev;
            break ;
        case Device::Wifi:
            m_wirelessDeviceList << dev;
            break ;
        default:
            break ;
        }
    }
}

