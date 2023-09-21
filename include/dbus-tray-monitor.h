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

#pragma once
#include <qt5-log-i.h>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDBusArgument>
#include <QMap>
#include <QObject>

#define KDE_STATUS_NOTIFIER_WATCHER_SERVICE        "org.kde.StatusNotifierWatcher"
#define KDE_STATUS_NOTIFIER_WATCHER_PATH           "/StatusNotifierWatcher"
#define FREEDESKTOP_DBUS_PROPERTIES_INTERFACE      "org.freedesktop.DBus.Properties"
#define FREEDESKTOP_DBUS_PROPERTIES_CHANGED_METHOD "PropertiesChanged"

#define KDE_STATUS_NOTIFIER_HOST "org.kde.StatusNotifierHost"

namespace KiranControlPanel
{
class DBusTrayMonitor : public QObject
{
    Q_OBJECT
public:
    DBusTrayMonitor(QObject *parent = nullptr)
        : QObject(parent),
          m_connection(QDBusConnection::sessionBus()),
          m_statusNotifierHostRegistered(false)
    {

        QDBusInterface trayWatcherInterface(KDE_STATUS_NOTIFIER_WATCHER_SERVICE, KDE_STATUS_NOTIFIER_WATCHER_PATH,
                                            KDE_STATUS_NOTIFIER_WATCHER_SERVICE, QDBusConnection::sessionBus());

        // 确认托盘服务Watcher是否存在，检测Watcher IsStatusNotifierHostRegistered是否为true(满足QDBusTray可用的条件，具体可参考Qt代码)
        if (trayWatcherInterface.isValid() && trayWatcherInterface.property("IsStatusNotifierHostRegistered").toBool())
        {
            m_statusNotifierHostRegistered = true;
        }
        else
        {
            // 托盘服务Watcher不存在/Host未注册/IsStatusNotifierHostRegistered属性未更新
            // 连接到DBus Daemon,处理Watcher属性变化信号
            m_connection.connect(KDE_STATUS_NOTIFIER_WATCHER_SERVICE,
                                 KDE_STATUS_NOTIFIER_WATCHER_PATH,
                                 FREEDESKTOP_DBUS_PROPERTIES_INTERFACE,
                                 FREEDESKTOP_DBUS_PROPERTIES_CHANGED_METHOD, 
                                 this, SLOT(onWatcherServicePropertyChanged(QDBusMessage)));
        }
    }

    ~DBusTrayMonitor() {}

    bool isStatusNotifierHostRegistered() const { return m_statusNotifierHostRegistered; }

private slots:
    void onWatcherServicePropertyChanged(QDBusMessage msg)
    {
        // 若Host已注册过，无需判断
        if (m_statusNotifierHostRegistered)
            return;

        QList<QVariant> args = msg.arguments();
        QVariantMap changedProps = qdbus_cast<QVariantMap>(args.at(1).value<QDBusArgument>());
        for (auto iter = changedProps.begin(); iter != changedProps.end(); iter++)
        {
            if (iter.key() != "IsStatusNotifierHostRegistered")
                continue;

            // IsStatusNotifierHostRegistered,属性变为true
            // 发出DBus托盘服务可用信号(只在托盘初始化时发出)
            // 后续属性变化不关注
            if (iter.value().toBool())
            {
                m_statusNotifierHostRegistered = true;
                KLOG_DEBUG() << "notifier host registered,dbus tray available!";
                emit dbusTrayAvailable();
            }
            break;
        }
    }

signals:
    void dbusTrayAvailable();

private:
    QDBusConnection m_connection;
    bool m_statusNotifierHostRegistered;
};

static bool isDBusTrayAvailable()
{
    bool dbusTrayAvailable = false;
        DBusTrayMonitor conn;
        if (conn.isStatusNotifierHostRegistered())
            dbusTrayAvailable = true;
    return dbusTrayAvailable;
}

}  // namespace KiranControlPanel