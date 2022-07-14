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
#include "status-notification.h"
#include "tray-widget.h"

TrayWidget::TrayWidget(QWidget *parent) : QWidget(parent)
{
    init();
}

TrayWidget::~TrayWidget()
{
}

void TrayWidget::init()
{
    initUI();
    initConnection();
}

void TrayWidget::initUI()
{
}

void TrayWidget::initConnection()
{
    connect(notifier(), &Notifier::activeConnectionAdded, this, &TrayWidget::handleActiveConnectionAdded);
    connect(notifier(), &Notifier::activeConnectionRemoved, this, &TrayWidget::handleActiveConnectionRemoved);

    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, this, &TrayWidget::handleNotifierConnectionAdded);

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

void TrayWidget::distributeNotifeir()
{

}

void TrayWidget::handleNotifierConnectionAdded(const QString &path)
{
}

void TrayWidget::handleNotifierConnectionRemoved(const QString &path)
{
}

void TrayWidget::handleActiveConnectionAdded(const QString &activepath)
{
}

void TrayWidget::handleActiveConnectionRemoved(const QString &activepath)
{
}

void TrayWidget::handleActiveConnectionStateChanged(ActiveConnection::State state)
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

void TrayWidget::handleStateActivating(const QString &activatedPath)
{

}

void TrayWidget::handleStateActivated(const QString &activatedPath)
{

}

void TrayWidget::handleStateDeactivated(const QString &activatedPath)
{

}

void TrayWidget::getDeviceList(Device::Type deviceType)
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

void TrayWidget::handleDeviceStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason)
{

}
