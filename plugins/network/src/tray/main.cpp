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

#include <kiran-application.h>
#include <qt5-log-i.h>
#include <QApplication>
#include <QTimer>
#include <QTranslator>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include "manager-tray.h"
#include "config.h"
#define MAX_WAIT_COUNTS 10

int main(int argc, char *argv[]) {
    KiranApplication a(argc, argv);
    klog_qt5_init("", "kylinsec-session", "kiran-cpanel-network", "kiran-cpanel-network");

    KLOG_INFO() << "autostart!";
    QTranslator translator;
    if (translator.load(QLocale(), "kiran-cpanel-network", ".", TRANSLATE_PREFIX, ".qm"))
    {
        a.installTranslator(&translator);
        KLOG_INFO() << "installTranslator load:" << a.installTranslator(&translator);
    }
    else
        KLOG_INFO() << "installTranslator failed";

    ManagerTray *tray = nullptr;
    QTimer timer;
    timer.setInterval(1000);
    int waitCounts = 0;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
                         if (QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.StatusNotifierWatcher"))
                         {
                             KLOG_INFO() << "org.kde.StatusNotifierWatcher isServiceRegistered" << QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.StatusNotifierWatcher");
                             KLOG_INFO() << "QSystemTrayIcon::isSystemTrayAvailable():" << QSystemTrayIcon::isSystemTrayAvailable();

                             KLOG_INFO() << "init  ManagerTray";
                             tray = new ManagerTray;
                             KLOG_INFO() << "wait loop : new ManagerTray sucess ";
                             KLOG_INFO() << "currentDateTime:"<<QDateTime::currentDateTime();
                             timer.stop();
                         }
                         else
                         {
                             waitCounts++;
                             KLOG_INFO() << "waitCounts:" << waitCounts;
                             if (waitCounts > MAX_WAIT_COUNTS)
                             {
                                 KLOG_INFO() << "超过等待次数，程序退出";
                                 return QApplication::quit();
                             }
                         }
                     });

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.StatusNotifierWatcher"))
    {
        KLOG_INFO() << "org.kde.StatusNotifierWatcher isServiceRegistered" << QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.StatusNotifierWatcher");
        KLOG_INFO() << "QSystemTrayIcon::isSystemTrayAvailable():" << QSystemTrayIcon::isSystemTrayAvailable();
        KLOG_INFO() << "init  ManagerTray";
        tray = new ManagerTray;
        KLOG_INFO() << "new ManagerTray sucess ";
    }
    else
    {
        timer.start();
        KLOG_INFO() << "start wait loop";
        KLOG_INFO() << "currentDateTime:" <<  QDateTime::currentDateTime();
    }
    return QApplication::exec();
}
