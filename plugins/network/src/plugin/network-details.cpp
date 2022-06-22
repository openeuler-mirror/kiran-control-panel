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
* Author:     luoqing <luoqing@kylinsec.com.cn>
*/

#include "network-details.h"
#include "ui_network-details.h"
#include <qt5-log-i.h>

NetworkDetails::NetworkDetails(QWidget *parent) : QWidget(parent), ui(new Ui::NetworkDetails)
{
    ui->setupUi(this);
    initUI();
}

NetworkDetails::~NetworkDetails()
{
    delete ui;
}

void NetworkDetails::initUI()
{
    activeConnections();
}

void NetworkDetails::getAvailableDeviceList()
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


