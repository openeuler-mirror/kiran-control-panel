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

#ifndef KIRAN_CPANEL_NETWORK_STATUSNOTIFICATION_H
#define KIRAN_CPANEL_NETWORK_STATUSNOTIFICATION_H

#include <QObject>
#include <NetworkManagerQt/Manager>
using namespace NetworkManager;
struct ConnectionInfo;
class StatusNotification : public QObject
{
    Q_OBJECT
public:
    explicit StatusNotification(QObject *parent = nullptr);
    ~StatusNotification();
public slots:
    void ActiveConnectionStateNotify(ActiveConnection::State state,ConnectionInfo connectionInfo);
    void ActiveConnectionDeactivatedNotify(ActiveConnection::State state);
    void deviceStateNotify(Device::State newstate);
    void connectitonFailedNotify(const QString &connectionName);
};

#endif  // KIRAN_CPANEL_NETWORK_STATUSNOTIFICATION_H
