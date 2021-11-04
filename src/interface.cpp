/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "interface.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QFile>
#include <QLocale>
#include <QTranslator>
#include "config/config.h"
#include "dbus-interface/appearance-global-info.h"
#include "pages/font/fonts.h"
#include "pages/theme/themes.h"
#include "pages/wallpaper/wallpaper.h"
#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

KcpInterface::KcpInterface()
{
}

bool KcpInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpInterface::visibleSubItems()
{
    QStringList subItems;
    subItems << "Wallpaper"
             << "Themes"
             << "Fonts";
    return subItems;
}

QWidget* KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if (id == "Wallpaper")
    {
        widget = new Wallpaper();
    }
    else if (id == "Fonts")
    {
        widget = new Fonts();
    }
    else if (id == "Themes")
    {
        widget = new Themes();
    }
    m_currentWidget = widget;
    ///加载qss样式表
    QFile file(":/themes/style.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        m_currentWidget->setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        KiranMessageBox::message(nullptr, tr("warning"), tr("load qss file failed"), KiranMessageBox::Ok);
    }

    return m_currentWidget;
}

void KcpInterface::uninit()
{
    if (m_translator)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

int KcpInterface::init()
{
    //QString themesInfo;
    //    if (!AppearanceGlobalInfo::instance()->getAllThemes(APPEARANCE_THEME_TYPE_GTK, themesInfo) ||
    //        AppearanceGlobalInfo::instance()->getFont(APPEARANCE_FONT_TYPE_APPLICATION).isEmpty())
    //    {
    //        KLOG_DEBUG() << "Connect dbus service failed! ";
    //        return -1;
    //    }
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(APPEARANCE_DBUS_NAME).value())
    {
        KLOG_INFO() << "Connect appearance dbus service failed!";
        return -1;
    }
    //加载翻译文件
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-appearance",
                            ".",
                            TRANSLATION_DIR,
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel appearance load translation failed";
        m_translator->deleteLater();
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }
    return 0;
}
