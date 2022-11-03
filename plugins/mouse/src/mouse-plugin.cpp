/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#include "mouse-plugin.h"
#include "kcm-manager.h"
#include "mouse-subitem.h"
#include "touchPad_backEnd_proxy.h"
#include "touchpad-subitem.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/mouse-i.h>
#include <kiran-session-daemon/touchpad-i.h>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QTranslator>

MousePlugin::MousePlugin(QObject* parent)
    : QObject(parent)
{
}

MousePlugin ::~MousePlugin()
{
}

int MousePlugin::init(KiranControlPanel::PanelInterface* interface)
{
    KCMManager kcmManager;
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(MOUSE_DBUS_NAME).value() &&
        !QDBusConnection::sessionBus().interface()->isServiceRegistered(TOUCHPAD_DBUS_NAME).value())
    {
        KLOG_INFO() << "Connect mouse or touchpad dbus service failed!";
        return -1;
    }

    QSharedPointer<TouchPadBackEndProxy> touchPadInterface = kcmManager.getTouchPadInterface();
    bool hasTouchPad = touchPadInterface->has_touchpad();

    // 加载翻译文件
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
                            "/usr/share/kiran-control-panel/translations/",
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

    m_subitems.append(KiranControlPanel::SubItemPtr(new MouseSubItem()));

    if (hasTouchPad)
        m_subitems.append(KiranControlPanel::SubItemPtr(new TouchPadSubItem()));

    return 0;
}

void MousePlugin::uninit()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        m_translator->deleteLater();
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> MousePlugin::getSubItems()
{
    return m_subitems;
}
