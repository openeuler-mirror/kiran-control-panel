/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
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

#include "dbus-wrapper.h"
#include <kiran-session-daemon/keyboard-i.h>
#include "keyboard-backEnd-proxy.h"
//#include <kiran-session-daemon/>
DbusWrapper::DbusWrapper(QObject *parent)
{
    m_keyboardInterface = QSharedPointer<KeyboardBackEndProxy>(new KeyboardBackEndProxy(KEYBOARD_DBUS_NAME,
                                                                                        KEYBOARD_OBJECT_PATH,
                                                                                        QDBusConnection::sessionBus()));
}

DbusWrapper::~DbusWrapper()
{
}

bool DbusWrapper::isValidConnect()
{
    if (!m_keyboardInterface->isValid())
        return false;
    return true;
}

QSharedPointer<KeyboardBackEndProxy> DbusWrapper::getKeyboardInterface()
{
    return m_keyboardInterface;
}
