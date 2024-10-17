/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
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
#include "audio-system-tray.h"
#include "config.h"

#include <kiran-application.h>
#include <qt5-log-i.h>

#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QDBusReply>
#include <QDateTime>
#include <QFile>
#include <QTranslator>
#include "dbus-tray-monitor.h"
#include "logging-category.h"
#include <QScopedPointer> 

#define DBUS_SERVICE_KDE_STATUS_NOTIFIER_WATCHER "org.kde.StatusNotifierWatcher"

int main(int argc, char *argv[])
{
    KiranApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    klog_qt5_init("", "kylinsec-session", "kiran-cpanel-audio", "kiran-cpanel-audio");
    KLOG_INFO(qLcAudio) << "audio tray autostart!";

    QTranslator translator;
    if (translator.load(QLocale(), "kiran-control-panel", ".", TRANSLATE_PREFIX, ".qm"))
    {
        a.installTranslator(&translator);
        KLOG_DEBUG(qLcAudio) << "installTranslator load:" << a.installTranslator(&translator);
    }
    else
    {
        KLOG_WARNING() << "installTranslator failed";
    }

     QScopedPointer<AudioSystemTray> audioSystemTray;
    if (KiranControlPanel::isDBusTrayAvailable())
    {
        KLOG_INFO(qLcAudio) << KDE_STATUS_NOTIFIER_HOST << "is registered,create audio tray icon";
        audioSystemTray.reset(new AudioSystemTray);
    }
    else
    {
        KLOG_WARNING() << KDE_STATUS_NOTIFIER_HOST << "is not registered,wait";

        auto dBusTrayMonitor = new KiranControlPanel::DBusTrayMonitor();
        // clang-format off
        QObject::connect(dBusTrayMonitor, &KiranControlPanel::DBusTrayMonitor::dbusTrayAvailable, [&audioSystemTray]()
        {
            if(audioSystemTray.isNull())
            {
                KLOG_INFO(qLcAudio) << KDE_STATUS_NOTIFIER_HOST << "is registered,create audio tray icon";
                audioSystemTray.reset(new AudioSystemTray);
            } 
        });
        // clang-format on
    }
    return QApplication::exec();
}
