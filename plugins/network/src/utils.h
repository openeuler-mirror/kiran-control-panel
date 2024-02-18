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

#ifndef KIRAN_CPANEL_NETWORK_UTILS_H
#define KIRAN_CPANEL_NETWORK_UTILS_H

#include <NetworkManagerQt/Manager>
#include <QDebug>

namespace NetworkUtils
{

QPixmap trayIconColorSwitch(const QString &iconPath, const int iconSize = 16);

QPixmap trayIconColorSwitch(QPixmap pixmap);

NetworkManager::Device::List getDeviceList(NetworkManager::Device::Type type);

NetworkManager::Device::List getAvailableDeviceList(NetworkManager::Device::Type type);

NetworkManager::Device::List getManagedDeviceList(NetworkManager::Device::Type type);

NetworkManager::Connection::Ptr getAvailableConnectionBySsid(const QString &devicePath, const QString &ssid);

NetworkManager::ConnectionSettings::Ptr createWirelessConnectionSettings(const QString &ssid, const QString &devicePath, const QString &accessPointPath);

bool isAvailableConnection(const QString &devicePath, NetworkManager::Connection::Ptr connection);

NetworkManager::Connection::List getAvailableWiredConnections(const QString &devicePath);

bool isExistedActivatedDevice();

QDebug operator<<(QDebug dbg, NetworkManager::Device *device);

}  // namespace NetworkUtils

#endif  // KIRAN_CPANEL_NETWORK_UTILS_H
