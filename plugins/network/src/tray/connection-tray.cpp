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
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessSetting>
#include "connection-lists.h"
#include "connection-show-page.h"
#include "connection-tray.h"
#include "status-notification.h"

ConnectionTray::ConnectionTray(QWidget *parent) : QWidget(parent)
{
    init();
}

ConnectionTray::~ConnectionTray()
{
}

void ConnectionTray::init()
{
    initUI();
    initConnection();
}

void ConnectionTray::initUI()
{
}

void ConnectionTray::initConnection()
{
    connect(notifier(), &Notifier::activeConnectionAdded, this, &ConnectionTray::handleActiveConnectionAdded);
    connect(notifier(), &Notifier::activeConnectionRemoved, this, &ConnectionTray::handleActiveConnectionRemoved);

    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, this, &ConnectionTray::handleNotifierConnectionAdded);

    m_connectionRemovedTimer.setInterval(100);
    m_connectionRemovedTimer.setSingleShot(true);
    //connectionRemoved信号会激发两次，原因暂时未知，使用定时器使短时间内多次相同信号只调用一次槽函数
    connect(settingsNotifier(), &SettingsNotifier::connectionRemoved, [=](const QString &path) {
                m_connectionRemovePath = path;
                m_connectionRemovedTimer.start();
            });

    connect(&m_connectionRemovedTimer, &QTimer::timeout, [=]() {
                handleNotifierConnectionRemoved(m_connectionRemovePath);
            });

    /**
     * ActiveConnection::State::Activated信号会重复激发两次，但是由dbus发送的Activated信号并没有重复
     * 原因暂时未知，使用定时器使短时间内多次相同信号只调用一次槽函数
     * */
    m_StateActivatedTimer.setSingleShot(true);
    m_StateActivatedTimer.setInterval(100);
    connect(&m_StateActivatedTimer, &QTimer::timeout, [=](){
                KLOG_DEBUG() << "handleStateActivated:";
                handleStateActivated(m_activatedPath);
            });
}

void ConnectionTray::distributeNotifeir()
{

}

void ConnectionTray::handleNotifierConnectionAdded(const QString &path)
{
}

void ConnectionTray::handleNotifierConnectionRemoved(const QString &path)
{
}

void ConnectionTray::handleActiveConnectionAdded(const QString &activepath)
{
}

void ConnectionTray::handleActiveConnectionRemoved(const QString &activepath)
{
}

void ConnectionTray::handleActiveConnectionStateChanged(ActiveConnection::State state)
{
    auto activeConnection = qobject_cast<ActiveConnection* >(sender());
    m_activatedPath = activeConnection->path();
    QString id = activeConnection->id();

    switch (state)
    {
    case ActiveConnection::State::Unknown:
        KLOG_DEBUG() << "ActiveConnection::State::Unknown";
        break;
    case ActiveConnection::State::Activating:
        KLOG_DEBUG() << "ActiveConnection::State::Activating";
        handleStateActivating(m_activatedPath);
        break;
    case ActiveConnection::State::Activated:
        KLOG_DEBUG() << "ActiveConnection::State::Activated";
        m_StateActivatedTimer.start();
        break;
    case ActiveConnection::State::Deactivating:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivating";
        break;
    case ActiveConnection::State::Deactivated:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivated id:" << id;
        if(!id.isEmpty())
            StatusNotification::ActiveConnectionDeactivatedNotify(id);
        handleStateDeactivated(m_activatedPath);
        break;
    default:
        break;
    }
}

void ConnectionTray::handleStateActivating(const QString &activatedPath)
{

}

void ConnectionTray::handleStateActivated(const QString &activatedPath)
{

}

void ConnectionTray::handleStateDeactivated(const QString &activatedPath)
{

}

void ConnectionTray::getDeviceList(Device::Type deviceType)
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == deviceType)
        {
            m_deviceList << dev;
        }
    }
    KLOG_DEBUG() << "m_deviceList:" << m_deviceList;
    if (m_deviceList.isEmpty())
    {
        KLOG_DEBUG() << "No available devices were found";
    }
}

void ConnectionTray::handleDeviceStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason)
{

}
