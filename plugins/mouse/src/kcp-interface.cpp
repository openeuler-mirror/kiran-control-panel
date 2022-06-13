/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-mouse is licensed under Mulan PSL v2.
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
#include <kiran-session-daemon/mouse-i.h>
#include <kiran-session-daemon/touchpad-i.h>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QFile>
#include <QLocale>
#include <QTranslator>
#include "kcm-manager.h"
#include "mouse-page.h"
#include "touchPad_backEnd_proxy.h"
#include "touchpad-page.h"

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
    subItems << "MouseSettings";
    if (m_hasTouchPad)
    {
        subItems << "TouchPadSettings";
    }

    return subItems;
}

QWidget* KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if (id == "MouseSettings")
    {
        widget = new MousePage();
    }
    else if (id == "TouchPadSettings")
    {
        widget = new TouchPadPage();
    }
    m_currentWidget = widget;
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
    if (m_kcmManager)
    {
        delete m_kcmManager;
        m_kcmManager = nullptr;
    }
}

int KcpInterface::init()
{
    m_kcmManager = new KCMManager;
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(MOUSE_DBUS_NAME).value() &&
        !QDBusConnection::sessionBus().interface()->isServiceRegistered(TOUCHPAD_DBUS_NAME).value())
    {
        KLOG_INFO() << "Connect mouse or touchpad dbus service failed!";
        return -1;
    }

    QSharedPointer<TouchPadBackEndProxy> touchPadInterface = m_kcmManager->getTouchPadInterface();
    m_hasTouchPad = touchPadInterface->has_touchpad();
    touchPadInterface.clear();

    if (!m_hasTouchPad)
        KLOG_INFO() << "system don't have touchPad";
    //加载翻译文件
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-mouse",
                            ".",
                            "/usr/share/applications/kiran-cpanel-mouse",
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel mouse load translation failed";
        m_translator->deleteLater();
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }
    return 0;
}
