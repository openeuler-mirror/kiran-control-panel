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

#ifndef KIRAN_CPANEL_NETWORK_STATUSNOTIFICATION_H
#define KIRAN_CPANEL_NETWORK_STATUSNOTIFICATION_H

#include <NetworkManagerQt/Manager>
#include <QObject>
using namespace NetworkManager;
struct ConnectionInfo;
class StatusNotification : QObject
{
    Q_OBJECT
public:
    explicit StatusNotification(QObject *parent = nullptr);
    ~StatusNotification();

    static void ActiveConnectionActivatedNotify(ConnectionInfo connectionInfo);
    static void ActiveConnectionDeactivatedNotify(const QString &connectionName);

    static void connectitonFailedNotify(const QString &connectionPath);
    static void connectionDeleteNotify(const QString  &connectionName);

    static void deviceStateNotify(Device::State newstate);

};

#endif  // KIRAN_CPANEL_NETWORK_STATUSNOTIFICATION_H
