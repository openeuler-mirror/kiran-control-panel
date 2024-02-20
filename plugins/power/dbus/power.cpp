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
#include "power.h"
#include "gnome_session_manager_proxy.h"

PowerInterface *PowerInterface::m_instance = nullptr;

PowerInterface::PowerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : KSDPowerProxy(service, path, QDBusConnection::sessionBus()),
    m_gnomeSmProxy(new GnomeSMProxy("org.gnome.SessionManager","/org/gnome/SessionManager",QDBusConnection::sessionBus(),this))
{
    qRegisterMetaType<IdleAction>("IdleAction");
    qDBusRegisterMetaType<IdleAction>();
}

PowerInterface::~PowerInterface()
{
    if(m_gnomeSmProxy)
    {
        m_gnomeSmProxy->deleteLater();
    }
}

void PowerInterface::globalInit()
{
    m_instance = new PowerInterface("com.kylinsec.Kiran.SessionDaemon.Power", "/com/kylinsec/Kiran/SessionDaemon/Power", QDBusConnection::sessionBus());
}

bool PowerInterface::screenLockedWhenSuspend()
{
    return m_gnomeSmProxy->screenLockedWhenSuspend();
}

bool PowerInterface::LockScreenWhenSuspend(bool checked)
{
    m_gnomeSmProxy->setScreenLockedWhenSuspend(checked);
    return m_gnomeSmProxy->screenLockedWhenSuspend() == checked;
}
