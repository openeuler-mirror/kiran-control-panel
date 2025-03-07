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
#include "device-manager.h"
#include <NetworkManagerQt/Manager>
#include "logging-category.h"

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
DeviceManager::DeviceManager(QObject* parent)
    : QObject(parent)
{
}

void DeviceManager::init()
{
    // 加载所有当前已有网卡设备
    auto devices = networkInterfaces();
    for (auto device : devices)
    {
        if (device->type() == NetworkManager::Device::Ethernet)
        {
            processDeviceAdded(device->uni());
        }
        else if (device->type() == NetworkManager::Device::Wifi)
        {
            processDeviceAdded(device->uni());
        }
    }

    // 感知设备动态变化
    connect(notifier(), &Notifier::deviceAdded, this, &DeviceManager::processDeviceAdded);
    connect(notifier(), &Notifier::deviceRemoved, this, &DeviceManager::processDeviceRemoved);
}

void DeviceManager::processDeviceRemoved(const QString& devicePath)
{
    // NOTE: 设备变成不可用或设备被移除不手动移除processDeviceStateChanged信号槽连接
    if (m_managedWiredDevices.contains(devicePath))
    {
        m_managedWiredDevices.remove(devicePath);
        emit managedDeviceChanged();
        emit managedWiredDeviceRemoved(devicePath);
        KLOG_INFO(qLcNetwork) << "managed wired device removed" << devicePath;
    }
    else if (m_managedWirelessDevices.contains(devicePath))
    {
        m_managedWirelessDevices.remove(devicePath);
        emit managedDeviceChanged();
        emit managedWirelessDeviceRemoved(devicePath);
        KLOG_INFO(qLcNetwork) << "managed wireless device removed" << devicePath;
    }
}

void DeviceManager::processDeviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate)
{
    auto obj = sender();
    if (!obj)
    {
        return;
    }

    auto device = qobject_cast<NetworkManager::Device*>(obj);
    if (!device)
    {
        return;
    }

    auto interfaceName = device->interfaceName();
    auto devicePath = device->uni();

    auto isManaged = newstate > Device::Unmanaged;
    auto oldIsManaged = oldstate > Device::Unmanaged;

    if (isManaged == oldIsManaged)
    {
        return;
    }

    if (isManaged)
    {
        KLOG_INFO(qLcNetwork) << interfaceName << "become managed";
        processDeviceAdded(devicePath);
    }
    else
    {
        KLOG_INFO(qLcNetwork) << interfaceName << "become unmanaged";
        processDeviceRemoved(devicePath);
    }
}

void DeviceManager::processDeviceAdded(const QString& devicePath)
{
    auto device = NetworkManager::findNetworkInterface(devicePath);
    if (!device)
    {
        KLOG_WARNING(qLcNetwork) << "device added, can't find device by path: " << devicePath;
        return;
    }

    // 只处理Ethernet/Wifi设备
    auto type = device->type();
    if (type != Device::Ethernet &&
        type != Device::Wifi)
    {
        return;
    }

    auto deviceName = device->interfaceName();
    KLOG_INFO(qLcNetwork) << "device added:" << deviceName << devicePath;

    // 处理状态变为不可用或变成可用的设备
    connect(device.data(), &Device::stateChanged, this, &DeviceManager::processDeviceStateChanged, Qt::UniqueConnection);

    auto state = device->state();
    if (state <= Device::Unmanaged)
    {
        // 不接收Unmanaged/UnknownState设备，等待它们就绪
        KLOG_WARNING(qLcNetwork) << deviceName << "state is not ready, state: " << state;
        return;
    }

    // 已存在设备不重复发送信号
    if (m_managedWiredDevices.contains(devicePath) || m_managedWirelessDevices.contains(devicePath))
    {
        KLOG_WARNING(qLcNetwork) << device->interfaceName() << "already exists";
        return;
    }

    KLOG_INFO(qLcNetwork) << "device" << deviceName << "is ready" << devicePath;

    if (type == Device::Ethernet)
    {
        m_managedWiredDevices << devicePath;
        emit managedWiredDeviceAdded(devicePath);
    }
    else if (type == Device::Wifi)
    {
        m_managedWirelessDevices << devicePath;
        emit managedWirelessDeviceAdded(devicePath);
    }
    emit managedDeviceChanged();
}

DeviceManager* DeviceManager::instance_ = nullptr;
void DeviceManager::global_init()
{
    if (instance_ == nullptr)
    {
        instance_ = new DeviceManager();
        instance_->init();
    }
}

QStringList DeviceManager::managedWiredDeivces()
{
    return m_managedWiredDevices.values();
}

QStringList DeviceManager::managedWirelessDeivces()
{
    return m_managedWirelessDevices.values();
}

}  // namespace Network
}  // namespace Kiran
