/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#pragma once

#include <QSharedPointer>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

#include "logging-category.h"
#include "kiran_appearance_service.h"
namespace DBusWrapper
{
    typedef QSharedPointer<KiranAppearanceService>  KiranAppearanceServicePtr;

    static const char* KiranAppearanceServiceName = "com.kylinsec.Kiran.SessionDaemon.Appearance";
    static const char* KiranAppearanceServiceObjectPath = "/com/kylinsec/Kiran/SessionDaemon/Appearance";

    static inline KiranAppearanceServicePtr createKiranAppearancetServiceAPI()
    {
        if ( !QDBusConnection::sessionBus().interface()->isServiceRegistered(KiranAppearanceServiceName) )
        {
            KLOG_WARNING(qLcDbusWrapper) << "service:" << KiranAppearanceServiceName << ",is not registered!";
            return KiranAppearanceServicePtr();
        }

        return KiranAppearanceServicePtr(new KiranAppearanceService(KiranAppearanceServiceName,KiranAppearanceServiceObjectPath,QDBusConnection::sessionBus()));
    }
}