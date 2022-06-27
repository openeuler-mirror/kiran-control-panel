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

#include "status-notification.h"
#include <libnotify/notify.h>
#include <qt5-log-i.h>
#include "connection-lists.h"
#include <NetworkManagerQt/ActiveConnection>

StatusNotification::StatusNotification(QObject *parent) : QObject(parent)
{
    notify_init("StatusNotification");
}

StatusNotification::~StatusNotification()
{
    notify_uninit();
}

void StatusNotification::ActiveConnectionStateNotify(ActiveConnection::State state,ConnectionInfo connectionInfo)
{
    KLOG_DEBUG() << "ActiveConnectionStateNotify";
    QString id = connectionInfo.id;
    QString ssid = connectionInfo.wirelessInfo.ssid;
    QString summary, body, bodyStr, icon;

    if (state == ActiveConnection::Activated)
    {
        summary = tr("Connection activated");
        body = tr("You are now connected to the network \"%1\"");
        if (connectionInfo.isWireless)
        {
            bodyStr = body.replace("the", "WIFI").arg(ssid);
            int signal = connectionInfo.wirelessInfo.signalStrength;
            if (0 <= signal && signal < 25)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-1.svg";
            else if (25 <= signal && signal < 50)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-2.svg";
            else if (50 <= signal && signal < 75)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-3.svg";
            else if (75 <= signal && signal <= 100)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-4.svg";
        }
        else
        {
            bodyStr = body.arg(id);
            icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wired-connection.svg";
        }
        NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
        notify_notification_show(notify, nullptr);
        g_object_unref(G_OBJECT(notify));
    }
}

//xxx:deactivated时的通知待优化
void StatusNotification::ActiveConnectionDeactivatedNotify(ActiveConnection::State state)
{
  if (state == ActiveConnection::Deactivated)
    {
        KLOG_DEBUG() << "ActiveConnectionDeactivatedNotify";
        bool isWireless;
        QString id,ssid;
        QString summary, body, bodyStr, icon;

        ActiveConnection *activeConnection = qobject_cast<ActiveConnection* >(sender());
        if(activeConnection->type() == ConnectionSettings::Wireless)
        {
            isWireless = true;
            ssid =  activeConnection->id();
        }
        else
            id = activeConnection->id();


        summary = tr("Connection deactivated");
        body = tr("You have now disconnected the network \"%1\"");
        if (isWireless)
        {
            bodyStr = body.replace("the", "WIFI").arg(ssid);
            icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-error.svg";
        }
        else
        {
            bodyStr = body.arg(id);
            icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wired-disconnected.svg";
        }

        NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
        notify_notification_show(notify, nullptr);
        g_object_unref(G_OBJECT(notify));
    }
}

void StatusNotification::connectitonFailedNotify(const QString &connectionName)
{
    QString summary, body, bodyStr, icon;
    summary = tr("Connection Failed");
    body = tr("Failed to connect to the network \"%1\"");
    bodyStr = body.arg(connectionName);

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
}

void StatusNotification::deviceStateNotify(Device::State newstate)
{

}
