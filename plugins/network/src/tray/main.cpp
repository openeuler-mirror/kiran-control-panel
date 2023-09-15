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

#include <kiran-application.h>
#include <qt5-log-i.h>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QTranslator>
#include "config.h"
#include "network-tray.h"

#define DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER "org.kde.StatusNotifierWatcher"

int main(int argc, char* argv[])
{
    KiranApplication a(argc, argv);
    klog_qt5_init("", "kylinsec-session", "kiran-cpanel-network", "kiran-cpanel-network");

    KLOG_INFO() << "autostart!";

    QTranslator translator;
    if (translator.load(QLocale(), "kiran-cpanel-network", ".", TRANSLATE_PREFIX, ".qm"))
    {
        a.installTranslator(&translator);
        KLOG_DEBUG() << "installTranslator load:" << a.installTranslator(&translator);
    }
    else
    {
        KLOG_WARNING() << "installTranslator failed";
    }

    NetworkTray* tray = nullptr;
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER))
    {
        KLOG_DEBUG() << DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER << "is registered,create network tray icon";
        tray = new NetworkTray;
    }
    else
    {
        KLOG_WARNING() << DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER << "is not registered,wait";
        QDBusServiceWatcher* dbusServiceWatcher = new QDBusServiceWatcher;
        dbusServiceWatcher->setConnection(QDBusConnection::sessionBus());
        dbusServiceWatcher->addWatchedService(DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER);
        QObject::connect(dbusServiceWatcher, &QDBusServiceWatcher::serviceRegistered,
                         [&dbusServiceWatcher, &tray](const QString& service)
                         {
                             if (service != DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER)
                                 return;
                             KLOG_INFO() << DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER << "is registered,create network tray icon";
                             tray = new NetworkTray;
                             dbusServiceWatcher->removeWatchedService(DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER);
                         });
    }

    return QApplication::exec();
}
