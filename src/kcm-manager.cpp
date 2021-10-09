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
#include "KSMMouseProxy.h"
#include "KSMTouchPadProxy.h"
KCMManager::KCMManager(QObject *parent) : QObject(parent)
{
    m_mouseInterface = QSharedPointer<KSMMouseProxy>(new KSMMouseProxy(MOUSE_DBUS_NAME,
                                                                       MOUSE_OBJECT_PATH,
                                                                       QDBusConnection::sessionBus()));

    m_touchPadInterface = QSharedPointer<KSMTouchPadProxy>(new KSMTouchPadProxy(TOUCHPAD_DBUS_NAME,
                                                                                TOUCHPAD_OBJECT_PATH,
                                                                                QDBusConnection::sessionBus()));
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

QSharedPointer<KSMMouseProxy> KCMManager::getMouseInterface()
{
    return m_mouseInterface;
}

QSharedPointer<KSMTouchPadProxy> KCMManager::getTouchPadInterface()
{
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
