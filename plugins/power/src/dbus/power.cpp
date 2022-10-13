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
#include <kiran-session-daemon/power-i.h>
/*
 * Implementation of interface class PowerInterface
 */

PowerInterface *PowerInterface::instance()
{
    static QMutex mutex;
    static QScopedPointer<PowerInterface> pInst;

    if(Q_UNLIKELY(!pInst)){
        QMutexLocker locker(&mutex);
        if(pInst.isNull()){
            pInst.reset(new PowerInterface(QDBusConnection::sessionBus()));
        }
    }

    return pInst.data();
}

PowerInterface::PowerInterface(const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface("com.kylinsec.Kiran.SessionDaemon", "/com/kylinsec/Kiran/SessionDaemon/Power", staticInterfaceName(), connection, parent)
{
    qRegisterMetaType<IdleAction>("IdleAction");
    qDBusRegisterMetaType<IdleAction>();
}

PowerInterface::~PowerInterface()
{

}