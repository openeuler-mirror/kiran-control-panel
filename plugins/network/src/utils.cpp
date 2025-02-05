/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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

#include "utils.h"
#include <qt5-log-i.h>
#include <style-palette.h>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>

#include <QIcon>
using namespace NetworkManager;

QPixmap NetworkUtils::trayIconColorSwitch(const QString &iconPath, const int iconSize)
{
    // icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(iconSize, iconSize);
    if (Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}

QPixmap NetworkUtils::trayIconColorSwitch(QPixmap pixmap)
{
    if (!pixmap.isNull())
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        return QPixmap::fromImage(image);
    }
    return QPixmap();
}

NetworkManager::Device::List NetworkUtils::getDeviceList(NetworkManager::Device::Type type)
{
    const Device::List deviceList = networkInterfaces();
    Device::List list;
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == type)
        {
            list << dev;
        }
    }
    return list;
}

Device::List NetworkUtils::getAvailableDeviceList(NetworkManager::Device::Type type)
{
    const Device::List deviceList = networkInterfaces();
    Device::List list;
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == type)
        {
            if (dev->state() == Device::Unmanaged)
                continue;
            if (dev->state() == Device::Unavailable)
                continue;
            list << dev;
        }
    }
    return list;
}

Device::List NetworkUtils::getManagedDeviceList(NetworkManager::Device::Type type)
{
    const Device::List deviceList = networkInterfaces();
    Device::List list;
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == type)
        {
            if (dev->state() == Device::Unmanaged)
                continue;

            list << dev;
        }
    }
    return list;
}

QDebug NetworkUtils::operator<<(QDebug dbg, NetworkManager::Device *device)
{
    dbg << "device:" << device->interfaceName()
        << "path:" << device->uni()
        << "type:" << device->type()
        << "state:" << device->state();
    return dbg.maybeSpace();
}

NetworkManager::Connection::Ptr NetworkUtils::getAvailableConnectionBySsid(const QString &devicePath, const QString &ssid)
{
    auto device = findNetworkInterface(devicePath);
    if( device.isNull() )
    {
        return NetworkManager::Connection::Ptr();
    }

    Connection::List availableConnectionList = device->availableConnections();
    for (Connection::Ptr conn : availableConnectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::Wireless)
        {
            WirelessSetting::Ptr wirelessSetting = conn->settings()->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
            if (ssid == QString(wirelessSetting->ssid()))
            {
                return conn;
            }
        }
    }
    
    return NetworkManager::Connection::Ptr();
}

NetworkManager::ConnectionSettings::Ptr NetworkUtils::createWirelessConnectionSettings(const QString &ssid, const QString &devicePath, const QString &accessPointPath)
{
    ConnectionSettings::Ptr connectionSettings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wireless));
    connectionSettings->setId(ssid);
    connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

    WirelessSetting::Ptr wirelessSetting = connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wirelessSetting->setInitialized(true);
    wirelessSetting->setSsid(ssid.toUtf8());

    WirelessSecuritySetting::Ptr wirelessSecurity =
        connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
    wirelessSecurity->setInitialized(true);
    wirelessSetting->setSecurity(QStringLiteral("802-11-wireless-security"));

    Ipv4Setting::Ptr ipv4Setting = connectionSettings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    // 处理不同验证的情况
    // accessPointPath路径为空，对应隐藏网络情况，则默认为WpaPsk
    if (accessPointPath.isEmpty())
    {
        wirelessSetting->setHidden(true);
        WirelessSecuritySetting::KeyMgmt keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaPsk;
        wirelessSecurity->setKeyMgmt(keyMgmt);
    }
    else
    {
        auto device = findNetworkInterface(devicePath);
        WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice *>(device);
        AccessPoint::Ptr accessPoint = wirelessDevice->findAccessPoint(accessPointPath);
        AccessPoint::Capabilities capabilities = accessPoint->capabilities();
        AccessPoint::WpaFlags wpaFlags = accessPoint->wpaFlags();
        AccessPoint::WpaFlags rsnFlags = accessPoint->rsnFlags();

        WirelessSecuritySetting::KeyMgmt keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaNone;

        if (capabilities.testFlag(AccessPoint::Capability::Privacy) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
        {
            keyMgmt = WirelessSecuritySetting::KeyMgmt::Wep;
        }

        if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk))
        {
            keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaPsk;
        }

        if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
        {
            keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaEap;
        }
        wirelessSecurity->setKeyMgmt(keyMgmt);
    }

    return connectionSettings;
}

bool NetworkUtils::isAvailableConnection(const QString &devicePath, NetworkManager::Connection::Ptr connection)
{
    auto device = findNetworkInterface(devicePath);
    if (device->type() == Device::Ethernet)
    {
        auto settings = connection->settings();
        WiredSetting::Ptr wiredSetting = settings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
        QString mac = wiredSetting->macAddress().toHex(':').toUpper();
        auto wiredDevice = device->as<WiredDevice>();

        QString deviceMac = wiredDevice->permanentHardwareAddress();
        if( deviceMac.isEmpty() )
        {
            // USB共享网络时，某些设备存在PermHWAddress为空，此时取HWAddress
            deviceMac = wiredDevice->hardwareAddress();
        }

        if (mac == deviceMac || mac.isEmpty())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}

NetworkManager::Connection::List NetworkUtils::getAvailableWiredConnections(const QString &devicePath)
{
    auto device = findNetworkInterface(devicePath);
    if (!device || device->type() != Device::Ethernet)
    {
        return NetworkManager::Connection::List();
    }

    Connection::List availableConnections;
    if (device->state() > Device::Unavailable)
    {
        availableConnections = device->availableConnections();
    }
    else
    {
        auto wiredDevice = device->as<WiredDevice>();
        
        QString deviceHardwareAddress = wiredDevice->permanentHardwareAddress();
        if( deviceHardwareAddress.isEmpty() )
        {
            deviceHardwareAddress = wiredDevice->hardwareAddress();
        }

        auto allConnections = listConnections();
        for (auto connection : allConnections)
        {
            auto settings = connection->settings();
            if (settings->connectionType() != ConnectionSettings::Wired)
            {
                continue;
            }

            // 匹配配置以及设备中的Mac地址是否匹配
            WiredSetting::Ptr wiredSetting = settings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
            QString mac = wiredSetting->macAddress().toHex(':').toUpper();
            if ( !mac.isEmpty() && mac != deviceHardwareAddress )
            {
                continue;
            }

            // 匹配配置中指定的DEVICE是否和设备名匹配
            QString settingSpecifyDevice = settings->interfaceName();
            if ( !settingSpecifyDevice.isEmpty() && settingSpecifyDevice != device->interfaceName() )
            {
                continue;
            }

            availableConnections << connection;
        }
    }
    return availableConnections;
}

bool NetworkUtils::isExistedActivatedDevice()
{
    auto devices = networkInterfaces();
    Device::List activatedDeviceList;
    for (auto device : devices)
    {
        if (device->state() == Device::Activated)
        {
            activatedDeviceList << device;
        }
    }

    bool isExistedSupportedDevice = false;
    for (auto device : activatedDeviceList)
    {
        auto deviceType = device->type();
        QSet<Device::Type> deviceTypes = {
            Device::Ethernet,
            Device::Wifi,
            Device::Modem,
            Device::Bluetooth};

        if (deviceTypes.contains(deviceType))
        {
            isExistedSupportedDevice = true;
            break;
        }
    }
    return isExistedSupportedDevice;
}
