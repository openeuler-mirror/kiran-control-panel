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
#include "connection-lists.h"
using namespace NetworkManager;

void StatusNotification::connectitonFailedNotify()
{
    notify_init("StatusNotification");

    QString summary, body, icon;
    summary = tr("Connection Failed");
    body = tr("the network not found");

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), body.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

void StatusNotification::connectitonHiddenNetworkFailedNotify(const QString& ssid)
{
    notify_init("StatusNotification");

    QString summary, body, bodyStr, icon;
    summary = tr("Connection Failed");
    body = tr("The hidden network \"%1\" to be connected has been detected and exists in the network list");
    bodyStr = body.arg(ssid);

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

void StatusNotification::connectitonFailedNotify(const QString& connectionPath)
{
    notify_init("StatusNotification");

    Connection::Ptr connection = findConnection(connectionPath);

    QString summary, body, bodyStr, icon;
    summary = tr("Connection Failed");
    body = tr("Failed to connect to the network \"%1\"");
    bodyStr = body.arg(connection->name());

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

void StatusNotification::connectitonFailedNotifyByName(const QString& connectionName)
{
    notify_init("StatusNotification");

    QString summary, body, bodyStr, icon;
    summary = tr("Connection Failed");
    body = tr("Failed to connect to the network \"%1\"");
    bodyStr = body.arg(connectionName);

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

void StatusNotification::connectitonFailedNotifyByReason(const QString& reason)
{
    notify_init("StatusNotification");

    QString summary, icon;
    summary = tr("Connection Failed");

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), reason.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

void StatusNotification::ActiveConnectionActivatedNotify(NetworkConnectionInfo connectionInfo)
{
    notify_init("StatusNotification");

    KLOG_DEBUG() << "ActiveConnectionStateNotify";
    QString id = connectionInfo.id;
    QString ssid = connectionInfo.wirelessInfo.ssid;
    QString summary, body, bodyStr, icon;

    summary = tr("Connection activated");
    body = tr("You are now connected to the network \"%1\"");
    if (connectionInfo.isWireless)
    {
        bodyStr = body.replace("the", "WIFI").arg(ssid);
    }
    else
    {
        bodyStr = body.arg(id);
        //        icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wired-connection.svg";
    }
    icon = "";
    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

// xxx:deactivated时的通知待优化
void StatusNotification::ActiveConnectionDeactivatedNotify(const QString& connectionName)
{
    notify_init("StatusNotification");

    QString summary, body, bodyStr, icon;
    summary = tr("Connection deactivated");
    body = tr("You have now disconnected the network \"%1\"");
    bodyStr = body.arg(connectionName);
    icon = "";

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

void StatusNotification::connectionDeleteNotify(const QString& connectionName)
{
    notify_init("StatusNotification");

    QString summary, body, bodyStr, icon;
    summary = tr("Connection deleted");
    body = tr("The connection has been deleted \"%1\"");
    bodyStr = body.arg(connectionName);

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}

void StatusNotification::deviceStateNotify(Device::State newstate)
{
}

void StatusNotification::deviceStateChangeReasonNotify(NetworkManager::Device::StateChangeReason reason)
{
}
