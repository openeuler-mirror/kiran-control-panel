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
#include "ethernet-page.h"
#include <NetworkManagerQt/Manager>
#include <QScrollBar>
#include "connection-manager.h"
#include "device-manager.h"
#include "logging-category.h"
#include "ui_ethernet-page.h"
#include "wired-device-widget.h"

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{

enum EthernetPageIdx
{
    ETHERNET_PAGE_WIRED_DEVICES = 0,
    ETHERNET_PAGE_CONNECTION_SETTINGS = 1
};

EthernetPage::EthernetPage(QWidget* parent)
    : Page(PAGE_ETHERNET, parent),
      ui(new ::Ui::EthernetPage)
{
    ui->setupUi(this);
    init();
}

EthernetPage::~EthernetPage()
{
    delete ui;
}

void EthernetPage::reset()
{
    // 复位设备展示界面
    ui->stackedWidget->setCurrentIndex(ETHERNET_PAGE_WIRED_DEVICES);
    // 复位垂直滚动条
    ui->scrollArea_devices->verticalScrollBar()->setValue(0);
}

void EthernetPage::init()
{
    // 感知设备新增/删除
    connect(DM_INSTANCE, &DeviceManager::managedWiredDeviceAdded, this, &EthernetPage::processManagedDeviceAdded);
    connect(DM_INSTANCE, &DeviceManager::managedWiredDeviceRemoved, this, &EthernetPage::processManagedDeviceRemoved);

    // 处理请求跳转至编辑连接配置
    connect(PM_INSTANCE, &PageManager::requestEditConnectionSettings, this, &EthernetPage::editConncetionSettings);
    // 处理请求跳转至有线网卡设备展示页面
    connect(PM_INSTANCE, &PageManager::requestWiredDevicePage, this, &EthernetPage::requestWiredDevicePage);

    // 初始化有线网卡
    for (auto devicePath : DM_INSTANCE->managedWiredDeivces())
    {
        processManagedDeviceAdded(devicePath);
    }
}

QWidget* EthernetPage::findDeviceWidget(const QString& devicePath)
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

        auto deviceWidget = qobject_cast<WiredDeviceWidget*>(layoutWidget);
        if (!deviceWidget || deviceWidget->uni() != devicePath)
        {
            continue;
        }

        widget = deviceWidget;
        break;
    }
    return widget;
}

void EthernetPage::processManagedDeviceAdded(const QString& devicePath)
{
    auto device = findNetworkInterface(devicePath);
    if (!device || device->type() != Device::Ethernet)
    {
        return;
    }

    auto wiredDevice = qSharedPointerDynamicCast<WiredDevice>(device);
    auto deviceWidget = new WiredDeviceWidget(wiredDevice, this);
    ui->layout_scrollAreaDevicesContents->insertWidget(0, deviceWidget);

    KLOG_INFO(qLcNetwork) << "add managed device" << device->interfaceName() << "to ethernet page";
}

void EthernetPage::processManagedDeviceRemoved(const QString& devicePath)
{
    auto deviceWidget = findDeviceWidget(devicePath);
    if (deviceWidget)
    {
        delete deviceWidget;
        KLOG_INFO(qLcNetwork) << "remove managed device" << devicePath << "from ethernet page";
    }
}

void EthernetPage::editConncetionSettings(const QString& devicePath, const QString& connectionPath)
{
    ui->stackedWidget->setCurrentIndex(ETHERNET_PAGE_CONNECTION_SETTINGS);
    ui->page_connectionSettings->editConnectionSettings(devicePath, connectionPath);
}

void EthernetPage::requestWiredDevicePage()
{
    ui->stackedWidget->setCurrentIndex(ETHERNET_PAGE_WIRED_DEVICES);
}

}  // namespace Network
}  // namespace Kiran