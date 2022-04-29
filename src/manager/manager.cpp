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

#include "manager.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
Manager::Manager(QWidget *parent) : QWidget(parent)
{
}

Manager::~Manager()
{
}

void Manager::initNotifierConnection()
{
    //该信号并不能判断连接是否真正Connected/Activated,只能判断一个连接被加入到激活容器中
    connect(notifier(), &Notifier::activeConnectionAdded, this, &Manager::handleActiveConnectionAdded);
    connect(notifier(), &Notifier::activeConnectionRemoved, this, &Manager::handleActiveConnectionRemoved);

    //连接Wired时触发，而连接VPN时没有触发该信号，暂时不使用该信号
    //    connect(notifier(), &Notifier::statusChanged, [=](NetworkManager::Status status) {
    //      KLOG_DEBUG() << "NetworkManager::Status:" << status;
    //
    //    });

    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, this, &Manager::handleNotifierConnectionAdded);
    connect(settingsNotifier(), &SettingsNotifier::connectionRemoved, this, &Manager::handleNotifierConnectionRemoved);
}

void Manager::refreshConnectionLists()
{
    KLOG_DEBUG() << "Manager::refreshConnectionLists()";
}

void Manager::handleNotifierConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "Manager::handleNotifierConnectionAdded()";
}

void Manager::handleNotifierConnectionRemoved(const QString &path)
{
    KLOG_DEBUG() << "Manager::handleNotifierConnectionRemoved()";
}

void Manager::handleActiveConnectionAdded(const QString &activepath)
{
    KLOG_DEBUG() << "activeConnectionAdded:" << activepath;
}

void Manager::handleActiveConnectionRemoved(const QString &activepath)
{
    KLOG_DEBUG() << "activeConnectionRemoved:" << activepath;
}


//XXX:可以优化
void Manager::getDeviceInfo(Device::Type deviceType)
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == deviceType)
        {
            if(deviceType == Device::Ethernet)
            {
                auto device = qobject_cast<WiredDevice *>(dev);
                m_deviceMap.insert(device->permanentHardwareAddress(), device->uni());
            }
            else if(deviceType == Device::Wifi)
            {
                auto device = qobject_cast<WirelessDevice *>(dev);
                m_deviceMap.insert(device->permanentHardwareAddress(), device->uni());
            }
        }
    }
    KLOG_DEBUG() << "m_deviceMap:" << m_deviceMap;
    if (m_deviceMap.isEmpty())
    {
        KLOG_DEBUG() << "Wired device not found";
    }
}

void Manager::handleActiveConnectionStateChanged(ActiveConnection::State state, const QString &path)
{
    switch (state)
    {
    case ActiveConnection::State::Unknown:
        KLOG_DEBUG() << "ActiveConnection::State::Unknown";
        break;
    case ActiveConnection::State::Activating:
        KLOG_DEBUG() << "ActiveConnection::State::Activating";
        break;
    case ActiveConnection::State::Activated:
        KLOG_DEBUG() << "ActiveConnection::State::Activated";
        handleStateActivated(path);
        break;
    case ActiveConnection::State::Deactivating:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivating";
        break;
    case ActiveConnection::State::Deactivated:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivated";
        handleStateDeactivated(path);
        break;
    default:
        break;
    }
}

void Manager::handleStateActivated(const QString &activatedPath)
{
}

void Manager::handleStateDeactivated(const QString &deactivatedPath)
{
}
