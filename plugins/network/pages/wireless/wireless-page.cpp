/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#include "wireless-page.h"
#include <kiran-push-button.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <QProcess>
#include <QScrollBar>
#include "device-manager.h"
#include "logging-category.h"
#include "ui_wireless-page.h"
#include "wireless-device-widget.h"

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
WirelessPage::WirelessPage(QWidget* parent)
    : Page(PAGE_WIRELESS, parent),
      ui(new ::Ui::WirelessPage)
{
    ui->setupUi(this);
    init();
}

WirelessPage::~WirelessPage()
{
    delete ui;
}

void WirelessPage::reset()
{
#if 0
    // 考虑使用场景，复位时全部展开可能不太好用，暂时先不处理
    // 全部展开
    auto layout = ui->layout_scrollAreaDevicesContents;
    for (int i = 0; i < layout->count(); i++)
    {
        auto item = layout->itemAt(i);
        auto widget = item->widget();
        if (!widget)
        {
            continue;
        }
        auto deviceWidget = qobject_cast<WirelessDeviceWidget*>(widget);
        if( !deviceWidget )
        {
            continue;
        }

        if (!deviceWidget->getIsExpand())
        {
            deviceWidget->setIsExpand(true);
        }
    }
#endif
    // 垂直滚动条复位
    ui->scrollArea->verticalScrollBar()->setValue(0);
}

void WirelessPage::init()
{
    // 管理已知网络
    m_knownNetworksBtn = new KiranPushButton(this);
    KiranPushButton::setButtonType(m_knownNetworksBtn, KiranPushButton::BUTTON_Default);
    m_knownNetworksBtn->setText(tr("Managing known wireless networks"));
    m_knownNetworksBtn->setToolTip(tr("Managing known wireless networks"));
    ui->layout_scrollAreaDevicesContents->insertWidget(0, m_knownNetworksBtn);
    connect(m_knownNetworksBtn, &QPushButton::clicked, this, &WirelessPage::jumpToManageWirelessNetworkSettings);

    // 感知设备新增/删除
    connect(DM_INSTANCE, &DeviceManager::managedWirelessDeviceAdded, this, &WirelessPage::processManagedDeviceAdded);
    connect(DM_INSTANCE, &DeviceManager::managedWirelessDeviceRemoved, this, &WirelessPage::processManagedDeviceRemoved);

    // 初始化无线网卡设备控件
    for (auto devicePath : DM_INSTANCE->managedWirelessDeivces())
    {
        processManagedDeviceAdded(devicePath);
    }
}

QWidget* WirelessPage::findDeviceWidget(const QString& devicePath)
{
    QWidget* widget = nullptr;
    for (int i = 0; i < ui->layout_scrollAreaDevicesContents->count(); i++)
    {
        auto layoutItem = ui->layout_scrollAreaDevicesContents->itemAt(i);
        auto layoutWidget = layoutItem->widget();
        if (!layoutWidget)
        {
            continue;
        }

        auto deviceWidget = qobject_cast<WirelessDeviceWidget*>(layoutWidget);
        if (!deviceWidget || deviceWidget->uni() != devicePath)
        {
            continue;
        }

        widget = deviceWidget;
        break;
    }
    return widget;
}

void WirelessPage::processManagedDeviceAdded(const QString& devicePath)
{
    auto device = findNetworkInterface(devicePath);
    if (!device || device->type() != Device::Wifi)
    {
        KLOG_WARNING(qLcNetwork) << "device is't valid wireless device" << devicePath;
        return;
    }

    if (findDeviceWidget(devicePath))
    {
        KLOG_WARNING(qLcNetwork) << "wireless devices are present:" << devicePath << device->interfaceName();
        return;
    }

    auto wirelessDevice = qSharedPointerDynamicCast<WirelessDevice>(device);
    auto deviceWidget = new WirelessDeviceWidget(wirelessDevice, this);
    ui->layout_scrollAreaDevicesContents->insertWidget(0, deviceWidget);

    KLOG_INFO(qLcNetwork) << "add managed device" << device->interfaceName() << "to wireless page";
}

void WirelessPage::processManagedDeviceRemoved(const QString& devicePath)
{
    auto widget = findDeviceWidget(devicePath);
    if (widget)
    {
        delete widget;
        KLOG_INFO(qLcNetwork) << "remove managed device" << devicePath << "from wireless page";
    }
}

void WirelessPage::jumpToManageWirelessNetworkSettings()
{
    QProcess::startDetached("/usr/bin/nm-connection-editor", {"-t", "802-11-wireless", "-s"});
}

}  // namespace Network
}  // namespace Kiran