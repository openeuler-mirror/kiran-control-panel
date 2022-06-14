/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-audio is licensed under Mulan PSL v2.
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
#include "audio-system-tray.h"
#include "config.h"

#include <kiran-application.h>
#include <qt5-log-i.h>

#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QFile>
#include <QTimer>
#include <QTranslator>
#include <QDateTime>

#define MAX_WAIT_COUNTS 10

void loadStylesheet()
{
    QFile file(":/kcp-audio-themes/black_theme.qss");
    if (file.open(QIODevice::ReadOnly))
    {
        QString style = file.readAll();
        qApp->setStyleSheet(style);
        KLOG_INFO() << "load stylesheet success";
        file.close();
    }
    else
    {
        KLOG_INFO() << "load stylesheet failed";
    }
}

int main(int argc, char *argv[])
{
    KiranApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    klog_qt5_init("", "kylinsec-session", "kiran-cpanel-audio", "kiran-cpanel-audio");
    KLOG_INFO() << "autostart!";

    QTranslator translator;
    if (translator.load(QLocale(), "kiran-cpanel-audio", ".", AUDIO_TRANSLATION_DIR_INSTALL_PATH, ".qm"))
    {
        a.installTranslator(&translator);
        KLOG_INFO() << "installTranslator load:" << a.installTranslator(&translator);
    }
    else
        KLOG_INFO() << "installTranslator failed";
    loadStylesheet();

    AudioSystemTray *audioSystemTray = nullptr;

    QTimer timer;
    timer.setInterval(1000);
    int waitCounts = 0;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        if (QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.StatusNotifierWatcher"))
        {
            KLOG_INFO() << "org.kde.StatusNotifierWatcher isServiceRegistered" << QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.StatusNotifierWatcher");
            KLOG_INFO() << "SessionDaemon.Audio isServiceRegistered " << QDBusConnection::sessionBus().interface()->isServiceRegistered("com.kylinsec.Kiran.SessionDaemon.Audio");
            KLOG_INFO() << "QSystemTrayIcon::isSystemTrayAvailable():" << QSystemTrayIcon::isSystemTrayAvailable();

            KLOG_INFO() << "init  AudioSystemTray";
            audioSystemTray = new AudioSystemTray;
            KLOG_INFO() << "wait loop : new AudioSystemTray sucess ";
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
        KLOG_INFO() << "SessionDaemon.Audio isServiceRegistered " << QDBusConnection::sessionBus().interface()->isServiceRegistered("com.kylinsec.Kiran.SessionDaemon.Audio");
        KLOG_INFO() << "QSystemTrayIcon::isSystemTrayAvailable():" << QSystemTrayIcon::isSystemTrayAvailable();

        KLOG_INFO() << "init  AudioSystemTray";
        audioSystemTray = new AudioSystemTray;
        KLOG_INFO() << "new AudioSystemTray sucess ";
    }
    else
    {
        timer.start();
        KLOG_INFO() << "start wait loop";
        KLOG_INFO() << "currentDateTime:" <<  QDateTime::currentDateTime();
    }
    return QApplication::exec();
}
