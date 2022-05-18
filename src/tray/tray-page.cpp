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
#include "connection-show-page.h"
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <qt5-log-i.h>
#include "connection-lists.h"
TrayPage::TrayPage(QWidget *parent) : QWidget(parent)
{
    init();
}

TrayPage::~TrayPage()
{

}

void TrayPage::init()
{
    initUI();
    initConnection();
}

void TrayPage::initUI()
{
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setSpacing(6);
    m_verticalLayout->setContentsMargins(0, 0, 0, 0);

    m_multiDevicewidget = new QWidget(this);
    m_verticalDeviceWidgetLayout = new QVBoxLayout(m_multiDevicewidget);
    m_verticalDeviceWidgetLayout->setContentsMargins(10, 10, 10, 10);
    m_deviceLabel = new QLabel(m_multiDevicewidget);
    m_verticalDeviceWidgetLayout->addWidget(m_deviceLabel);

    m_deviceComboBox = new QComboBox(m_multiDevicewidget);
    m_verticalDeviceWidgetLayout->addWidget(m_deviceComboBox);

    m_verticalLayout->addWidget(m_multiDevicewidget);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollAreaWidgetContents = new QWidget();

    m_verticalScrollAreaWidgetContentsLayout = new QVBoxLayout(m_scrollAreaWidgetContents);
    m_verticalScrollAreaWidgetContentsLayout->setSpacing(0);
    m_verticalScrollAreaWidgetContentsLayout->setContentsMargins(0, 0, 0, 0);

    m_connectionLists = new ConnectionLists(m_scrollAreaWidgetContents);
    m_verticalScrollAreaWidgetContentsLayout->addWidget(m_connectionLists);

    m_scrollArea->setWidget(m_scrollAreaWidgetContents);
    m_verticalLayout->addWidget(m_scrollArea);
}

void TrayPage::initConnection()
{
    connect(notifier(), &Notifier::activeConnectionAdded, this, &TrayPage::handleActiveConnectionAdded);
    connect(notifier(), &Notifier::activeConnectionRemoved, this, &TrayPage::handleActiveConnectionRemoved);

    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, this, &TrayPage::handleNotifierConnectionAdded);

    m_connectionTimer.setInterval(100);
    m_connectionTimer.setSingleShot(true);
    //connectionRemoved信号会激发两次，原因暂时未知，使用定时器使短时间内多次相同信号只调用一次槽函数
    connect(settingsNotifier(), &SettingsNotifier::connectionRemoved, [=](const QString &path) {
                m_connectionRemovePath = path;
                m_connectionTimer.start();
            });

    connect(&m_connectionTimer, &QTimer::timeout, [=]() {
                handleNotifierConnectionRemoved(m_connectionRemovePath);
            });
}

void TrayPage::setMultiDeviceWidgetVisible(bool visible)
{
    m_multiDevicewidget->setVisible(visible);
}

void TrayPage::setDeviceLabel(const QString &label)
{
    m_deviceLabel->setText(label);
}

QPointer<ConnectionLists> TrayPage::getConnectionListsPtr()
{
    return QPointer<ConnectionLists>(m_connectionLists);
}

void TrayPage::handleNotifierConnectionAdded(const QString &path)
{
}
void TrayPage::handleNotifierConnectionRemoved(const QString &path)
{
}
void TrayPage::handleActiveConnectionAdded(const QString &activepath)
{
}
void TrayPage::handleActiveConnectionRemoved(const QString &activepath)
{
}

void TrayPage::handleActiveConnectionStateChanged(ActiveConnection::State state, const QString &path)
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

void TrayPage::handleStateActivated(const QString &activatedPath)
{

}
void TrayPage::handleStateDeactivated(const QString &deactivatedPath)
{

}

void TrayPage::getDeviceList(Device::Type deviceType)
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
