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
#include "dbus-tray-monitor.h"
#include "network-tray.h"

int main(int argc, char* argv[])
{
    KiranApplication a(argc, argv);
    klog_qt5_init("", "kylinsec-session", "kiran-cpanel-network", "kiran-cpanel-network");

    KLOG_INFO() << "autostart!";

    QTranslator translator;
    if (translator.load(QLocale(), "kiran-control-panel", ".", TRANSLATE_PREFIX, ".qm"))
    {
        a.installTranslator(&translator);
        KLOG_DEBUG() << "installTranslator load:" << a.installTranslator(&translator);
    }
    else
    {
        KLOG_WARNING() << "installTranslator failed";
    }

    NetworkTray* tray = nullptr;
    if (KiranControlPanel::isDBusTrayAvailable())
    {
        KLOG_DEBUG() << KDE_STATUS_NOTIFIER_HOST << "is registered,create network tray icon";
        tray = new NetworkTray;
    }
    else
    {
        KLOG_WARNING() << KDE_STATUS_NOTIFIER_HOST << "is not registered,wait";

        auto dBusTrayMonitor = new KiranControlPanel::DBusTrayMonitor();
        QObject::connect(dBusTrayMonitor, &KiranControlPanel::DBusTrayMonitor::dbusTrayAvailable, [&tray]()
                         {
                            if(tray == nullptr)
                            {
                                KLOG_DEBUG() << KDE_STATUS_NOTIFIER_HOST << "is registered,create network tray icon";
                                tray = new NetworkTray;
                            } 
                        });
    }

    return QApplication::exec();
}
