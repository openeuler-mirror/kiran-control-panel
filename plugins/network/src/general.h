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
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#ifndef KIRAN_CPANEL_NETWORK_GENERAL_H
#define KIRAN_CPANEL_NETWORK_GENERAL_H

#include <QString>

#define PROPERTY_NETWORK_CONNECTION_INFO "NetworkConnectionInfo"
#define TRAY_ITEM_NORAML_HIEGHT 50
#define MAX_WAIT_COUNTS 10
#define PLUGIN_ITEM_WIDGET_HEIGHT 36

struct WirelessConnectionInfo
{
    int signalStrength = 0;
    QString accessPointPath;
    QString ssid;
    bool securitySetting;
};

struct NetworkConnectionInfo
{
    QString id;
    QString uuid;
    QString connectionPath;
    QString devicePath;
    QString activeConnectionPath;
    bool isWireless = false;
    WirelessConnectionInfo wirelessInfo;
    int type;
};

#endif  // KIRAN_CPANEL_NETWORK_GENERAL_H