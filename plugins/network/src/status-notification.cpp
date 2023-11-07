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

#include "status-notification.h"
#include <libnotify/notify.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Settings>
#include "connection-list.h"
using namespace NetworkManager;

void StatusNotification::connectitonFailedNotify()
{
    QString summary, body;
    summary = tr("Connection Failed");
    body = tr("the network not found");
    generalNotify(summary,body);
}

void StatusNotification::connectitonHiddenNetworkFailedNotify(const QString& ssid)
{
    QString summary, body, bodyStr;
    summary = tr("Connection Failed");
    body = tr("The hidden network \"%1\" to be connected has been detected and exists in the network list");
    bodyStr = body.arg(ssid);
    generalNotify(summary,bodyStr);
}

void StatusNotification::connectitonFailedNotify(const QString& connectionPath)
{
    Connection::Ptr connection = findConnection(connectionPath);

    QString summary, body, bodyStr;
    summary = tr("Connection Failed");
    body = tr("Failed to connect to the network \"%1\"");
    bodyStr = body.arg(connection->name());
    generalNotify(summary,bodyStr);
}

void StatusNotification::connectitonFailedNotifyByName(const QString& connectionName)
{
    QString summary, body, bodyStr;
    summary = tr("Connection Failed");
    body = tr("Failed to connect to the network \"%1\"");
    bodyStr = body.arg(connectionName);
    generalNotify(summary,bodyStr);
}

void StatusNotification::connectitonFailedNotifyByReason(const QString& reason)
{
    QString summary = tr("Connection Failed");
    generalNotify(summary,reason);
}

void StatusNotification::ActiveConnectionActivatedNotify(const QString &connectionName)
{
    KLOG_DEBUG() << "ActiveConnectionStateNotify";
    QString summary, body;
    summary = tr("Connection activated");
    body = tr("You are now connected to the network \"%1\"").arg(connectionName);
  
    generalNotify(summary,body);
}

// xxx:deactivated时的通知待优化
void StatusNotification::ActiveConnectionDeactivatedNotify(const QString& connectionName)
{
    QString summary, body, bodyStr;
    summary = tr("Connection deactivated");
    body = tr("You have now disconnected the network \"%1\"");
    bodyStr = body.arg(connectionName);
    generalNotify(summary,bodyStr);
}

void StatusNotification::connectionDeleteNotify(const QString& connectionName)
{
    QString summary, body, bodyStr;
    summary = tr("Connection deleted");
    body = tr("The connection has been deleted \"%1\"");
    bodyStr = body.arg(connectionName);
    generalNotify(summary,bodyStr);
}

void StatusNotification::deviceStateNotify(Device::State newstate)
{
}

void StatusNotification::deviceStateChangeReasonNotify(NetworkManager::Device::StateChangeReason reason)
{
}

void StatusNotification::generalNotify(const QString& summary, const QString& body, const QString& icon)
{
    notify_init("StatusNotification");

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), body.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}
