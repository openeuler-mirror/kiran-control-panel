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

#include "kcm-manager.h"
#include <kiran-session-daemon/mouse-i.h>
#include <kiran-session-daemon/touchpad-i.h>
#include <QDBusConnection>
#include "mouse_backEnd_proxy.h"
#include "touchPad_backEnd_proxy.h"
KCMManager::KCMManager(QObject *parent) : QObject(parent)
{
}

KCMManager::~KCMManager()
{
}

bool KCMManager::isValidConnect()
{
    if (!m_mouseInterface->isValid() || !m_touchPadInterface->isValid())
        return false;
    return true;
}

QSharedPointer<MouseBackEndProxy> KCMManager::getMouseInterface()
{
    m_mouseInterface = QSharedPointer<MouseBackEndProxy>(new MouseBackEndProxy(MOUSE_DBUS_NAME,
                                                                               MOUSE_OBJECT_PATH,
                                                                               QDBusConnection::sessionBus()));

    return m_mouseInterface;
}

QSharedPointer<TouchPadBackEndProxy> KCMManager::getTouchPadInterface()
{
    m_touchPadInterface = QSharedPointer<TouchPadBackEndProxy>(new TouchPadBackEndProxy(TOUCHPAD_DBUS_NAME,
                                                                                        TOUCHPAD_OBJECT_PATH,
                                                                                        QDBusConnection::sessionBus()));
    return m_touchPadInterface;
}

bool KCMManager::hasTouchPad()
{
    if (m_touchPadInterface->has_touchpad())
    {
        return true;
    }
    return false;
}
