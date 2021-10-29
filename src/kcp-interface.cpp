/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keyboard is licensed under Mulan PSL v2.
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

#include "kcp-interface.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-session-daemon/keyboard-i.h>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QFile>
#include <QLocale>
#include <QTranslator>
#include "config/config.h"
#include "dbus-wrapper/dbus-wrapper.h"
#include "general-page.h"
#include "layout-page.h"

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
    subItems << "GeneralOptions"
             << "KeyboardLayout";

    return subItems;
}

QWidget* KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if (id == "GeneralOptions")
    {
        widget = new GeneralPage;
    }
    else if (id == "KeyboardLayout")
    {
        widget = new LayoutPage();
    }
    m_currentWidget = widget;

    //加载样式表
    QFile file(":/style/style.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        m_currentWidget->setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        KiranMessageBox::message(nullptr, "warning", "Open failed", KiranMessageBox::Ok);
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
    if (m_dbusWrapper)
    {
        delete m_dbusWrapper;
        m_dbusWrapper = nullptr;
    }
}

int KcpInterface::init()
{
    m_dbusWrapper = new DbusWrapper;
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(KEYBOARD_DBUS_NAME).value())
    {
        KLOG_INFO() << "Connect keyboard dbus service failed!";
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
                            "kiran-cpanel-keyboard",
                            ".",
                            TRANSLATIONS_FILE_DIR,
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel keyboard load translation failed";
        m_translator->deleteLater();
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }

    return 0;
}
