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

#include "device-list.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <QEvent>
#include "device-available-connection-widget.h"
#include "signal-forward.h"
#include "utils.h"
#include "logging-category.h"

using namespace NetworkManager;
using namespace NetworkUtils;

DeviceList::DeviceList(QWidget *parent) : QWidget(parent)
{
}

DeviceList::DeviceList(NetworkManager::Device::Type type, QWidget *parent) : QWidget(parent)
{
    init(type);
}

DeviceList::~DeviceList()
{
}

void DeviceList::init(NetworkManager::Device::Type type)
{
    m_deviceType = type;
    initUI();
    initConnect();

    auto managedDeviceList = NetworkUtils::getManagedDeviceList(type);
    for (auto device : managedDeviceList)
    {
        addDevice(device->uni());
    }

    // 在最后添加Spacer
    m_verticalSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_widgetContentsLayout->addItem(m_verticalSpacer);

    if (m_deviceType != Device::Wifi)
    {
        return;
    }

    for (auto device : managedDeviceList)
    {
        WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice *>(device);
        QDBusPendingReply<> replyRequestScan = wirelessDevice->requestScan();
        replyRequestScan.waitForFinished();
        if (replyRequestScan.isError())
        {
            KLOG_DEBUG(qLcNetwork) << "wireless Device:" << wirelessDevice << " requestScan error:" << replyRequestScan.error();
            continue;
        }
        KLOG_DEBUG(qLcNetwork) << "wireless Device:" << wirelessDevice << " requestScan reply:" << replyRequestScan.reply();
    }
}

void DeviceList::addWidget(QWidget *widget)
{
    m_widgetContentsLayout->insertWidget(0, widget);
    m_itemWidgetList << widget;
}

void DeviceList::removeWidget(QWidget *widget)
{
    m_widgetContentsLayout->removeWidget(widget);
    m_itemWidgetList.removeOne(widget);
    widget->deleteLater();
}

void DeviceList::addDevice(const QString &devicePath)
{
    Device::Ptr device = findNetworkInterface(devicePath);
    if ( device.isNull() )
    {
        KLOG_ERROR(qLcNetwork) << "failed to add device, failed to find device:" << devicePath;
        return;
    }

    if( m_deviceType != device->type() )
    {
        return;
    }

    if ( m_managedDevicePaths.contains(devicePath) )
    {
        KLOG_WARNING(qLcNetwork) << "device has been added:" << device->type() 
                                                             << device->interfaceName() 
                                                             << device->uni();
        return;
    }

    KLOG_INFO(qLcNetwork) << "add new device:" << device->type()
                                               << device->interfaceName() 
                                               << device->uni();
    auto widget = new DeviceAvailableConnectionWidget(device, m_scrollAreaWidgetContents);
    addWidget(widget);
    m_managedDevicePaths << devicePath;
}

void DeviceList::removeDevice(const QString &devicePath)
{
    if (!m_managedDevicePaths.contains(devicePath))
    {
        return;
    }

    for (auto item : m_itemWidgetList)
    {
        auto deviceItem = qobject_cast<DeviceAvailableConnectionWidget *>(item);
        if (deviceItem->devicePath() != devicePath)
        {
            continue;
        }

        removeWidget(deviceItem);
        m_managedDevicePaths.removeOne(devicePath);
        KLOG_INFO(qLcNetwork) << "removed device:" << devicePath;
        return;
    }
}

QWidget *DeviceList::itemWidget(int row)
{
    return m_itemWidgetList.value(row);
}

QList<QWidget *> DeviceList::itemWidgetList()
{
    return m_itemWidgetList;
}

bool DeviceList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_scrollArea)
    {
        if (event->type() == QEvent::Resize)
        {
            m_scrollAreaWidgetContents->setMaximumWidth(this->size().width());
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void DeviceList::initUI()
{
    m_verticalLayout = new QVBoxLayout(this);

    QString text;
    if (m_deviceType == Device::Ethernet)
    {
        text = tr("Wired Network Adapter");
    }
    else
    {
        text = tr("Wireless Network Adapter");
    }
    m_title = new QLabel(text, this);
    m_horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_horizontalLayout = new QHBoxLayout();
    m_horizontalLayout->addWidget(m_title);
    m_horizontalLayout->addItem(m_horizontalSpacer);

    m_verticalLayout->addLayout(m_horizontalLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_scrollAreaWidgetContents = new QWidget();
    m_widgetContentsLayout = new QVBoxLayout(m_scrollAreaWidgetContents);
    m_widgetContentsLayout->setContentsMargins(0, 0, 0, 0);
    m_widgetContentsLayout->setSpacing(8);

    // QScrollArea::setWidget会更改传入的widget的parentWidget
    m_scrollArea->setWidget(m_scrollAreaWidgetContents);
    m_scrollArea->setAlignment(Qt::AlignTop);
    m_scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_scrollArea->installEventFilter(this);

    m_verticalLayout->addWidget(m_scrollArea);
    m_verticalLayout->setContentsMargins(0, 0, 0, 0);
    m_verticalLayout->setSpacing(8);

    m_verticalLayout->setStretch(1, 1);
}

void DeviceList::initConnect()
{
    connect(SignalForward::instance(), &SignalForward::wiredDeviceAdded, this, &DeviceList::addDevice);
    connect(SignalForward::instance(), &SignalForward::wirelessDeviceAdded, this, &DeviceList::addDevice);

    connect(notifier(), &Notifier::deviceRemoved, this, &DeviceList::removeDevice);
}
