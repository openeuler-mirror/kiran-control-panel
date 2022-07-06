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

#include "network-plugin.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <QCoreApplication>
#include "details-page.h"
#include "vpn-manager.h"
#include "wired-manager.h"
#include "wireless-manager.h"

using namespace NetworkManager;
#define MAX_WAIT_COUNTS 10

NetworkPlugin::NetworkPlugin()
{
}

NetworkPlugin::~NetworkPlugin()
{
}

int NetworkPlugin::init()
{
    initSubItemsList();
    initConnect();
    loadTranslator();
    return 0;
}

void NetworkPlugin::uninit()
{
}

QWidget *NetworkPlugin::getSubItemWidget(QString subItemName)
{
    QWidget *widget = nullptr;

    if(subItemName.contains("Wired Connection"))
    {
        QString devicePath;
        QChar lastChar = subItemName.back();
        //若最后一位为数字，则代表多设备的情况
        if(lastChar.isDigit())
        {
            QString numStr(lastChar);
            int num = numStr.toInt();
            devicePath = m_wiredDevicePathList.at(num);
        }
        else
        {
            devicePath = m_wiredDevicePathList.at(0);
        }
        widget = new WiredManager(devicePath);
    }

    if(subItemName.contains("Wireless Connection"))
    {
        QString devicePath;
        QChar lastChar = subItemName.back();
        //若最后一位为数字，则代表多设备的情况
        if(lastChar.isDigit())
        {
            QString numStr(lastChar);
            int num = numStr.toInt();
            devicePath = m_wirelessDevicePathList.at(num);
        }
        else
        {
            devicePath = m_wirelessDevicePathList.at(0);
        }
        widget = new WirelessManager(devicePath);
    }

    if(subItemName == "VPN")
    {
        widget = new VpnManager();
    }

    if(subItemName == "Network Details")
    {
        widget = new DetailsPage();
    }
    return widget;
}

bool NetworkPlugin::haveUnsavedOptions()
{
    return false;
}

QStringList NetworkPlugin::visibleSubItems()
{
    return m_subItemsList;
}

void NetworkPlugin::initSubItemsList()
{
    getAvailableDeviceList();

    if (m_wiredDevicePathList.count() > 1)
    {
        QString subItemName = tr("Wired Connection %1");
        for (int i = 0; i < m_wiredDevicePathList.count(); ++i)
        {
            QString subItemNameStr = subItemName.arg(i + 1);
        }
    }
    else if (m_wiredDevicePathList.count() == 1)
    {
        m_subItemsList << "Wired Connection";
    }
    else  // m_wiredDeviceList.count() == 0
    {
    }

    if (m_wirelessDevicePathList.count() > 1)
    {
        QString subItemName = tr("Wireless Connection %1");
        for (int i = 0; i < m_wirelessDevicePathList.count(); ++i)
        {
            QString subItemNameStr = subItemName.arg(i + 1);
        }
    }
    else if (m_wirelessDevicePathList.count() == 1)
    {
        m_subItemsList << "Wireless Connection";
    }
    else  // m_wiredDeviceList.count() == 0
    {
    }

    m_subItemsList << "VPN"
                   << "Network Details";
}

void NetworkPlugin::getAvailableDeviceList()
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        if (dev->state() == Device::Unavailable)
            continue;

        switch (dev->type())
        {
        case Device::Ethernet:
            m_wiredDevicePathList << dev->uni();
            break;
        case Device::Wifi:
            m_wirelessDevicePathList << dev->uni();
            break;
        default:
            break;
        }
    }
}

void NetworkPlugin::initConnect()
{
    m_Timer.setInterval(500);
    m_Timer.setSingleShot(true);
    waitCounts = 1;
    connect(&m_Timer, &QTimer::timeout, [=]()
            {
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                if(device->managed())
                {
                    handleDeviceAdded(m_addDevicePath);
                    m_Timer.stop();
                }
                else
                {
                    KLOG_INFO() << "this device interface is not ready";
                    m_Timer.start();
                }
                waitCounts++;
                if(waitCounts > MAX_WAIT_COUNTS)
                {
                    KLOG_INFO() << "This device is not currently managed by NetworkManager";
                    m_Timer.stop();
                } });

    //Note:新设备插入后，需要等待一段时间，Device::List networkInterfaces() 来不及更新
    connect(notifier(), &Notifier::deviceAdded, [=](const QString &uni)
            {
                m_addDevicePath = uni;
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                KLOG_DEBUG() << "device->availableConnections().isEmpty():" << device->availableConnections().isEmpty();
                KLOG_DEBUG() << "device->managed():" << device->managed();
                if(!device->isValid())
                {
                    KLOG_INFO() << "this device interface is invalid!";
                    return ;
                }
                if(device->managed())
                    handleDeviceAdded(m_addDevicePath);
                else
                {
                    KLOG_INFO() << "this device interface is not ready";
                    m_Timer.start();
                    KLOG_INFO() << "wait counts:" << waitCounts;
                } });

    connect(notifier(), &Notifier::deviceRemoved, this,&NetworkPlugin::handleDeviceRemoved);
}


void NetworkPlugin::loadTranslator()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator(qApp);
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-audio",
                            ".",
                            "",
                            ".qm"))
    {
        KLOG_ERROR() << "can't load translator";
        delete m_translator;
        m_translator = nullptr;
    }
    else
    {
        qApp->installTranslator(m_translator);
    }
}

void NetworkPlugin::handleDeviceAdded(const QString &devicePath)
{
    Device::Ptr device = findNetworkInterface(devicePath);
    if (device->type() == Device::Ethernet)
    {

    }
    else if (device->type() == Device::Wifi)
    {

    }

    initSubItemsList();
    emit visibleSubItemsChanged();
}

// XXX:当device被移除时，由于设备对象可能已经被删除，所以并不能通过findNetworkInterface(path)找到该设备接口，进而知道被删除的设备类型
void NetworkPlugin::handleDeviceRemoved(const QString &devicePath)
{
    initSubItemsList();
    emit visibleSubItemsChanged();
}
