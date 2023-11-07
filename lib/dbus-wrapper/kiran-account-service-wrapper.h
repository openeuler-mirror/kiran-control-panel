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
#include "kiran_account_service.h"
#include "kiran_account_service_user.h"

namespace DBusWrapper
{
    typedef QSharedPointer<KiranAccountService>  KiranAccountServicePtr;
    typedef QSharedPointer<KiranAccountServiceUser> KiranAccountServiceUserPtr;

    static const char* KiranAccountServiceName = "com.kylinsec.Kiran.SystemDaemon.Accounts";
    static const char* KiranAccountServiceObjectPath = "/com/kylinsec/Kiran/SystemDaemon/Accounts";

    static inline KiranAccountServicePtr createKiranAccountServiceAPI()
    {
        if ( !QDBusConnection::systemBus().interface()->isServiceRegistered(KiranAccountServiceName) )
        {
            KLOG_WARNING(qLcDbusWrapper) << "service:" << KiranAccountServiceName << ",is not registered!";
        }

        return KiranAccountServicePtr(new KiranAccountService(KiranAccountServiceName,KiranAccountServiceObjectPath,QDBusConnection::systemBus()));
    }

    static inline KiranAccountServiceUserPtr createKiranAccountServiceUserAPI(const QString& userObjectPath)
    {
        if ( !QDBusConnection::systemBus().interface()->isServiceRegistered(KiranAccountServiceName) )
        {
            KLOG_WARNING(qLcDbusWrapper) << "service:" << KiranAccountServiceName << ",is not registered!";
        }

        return KiranAccountServiceUserPtr(new KiranAccountServiceUser(KiranAccountServiceName,userObjectPath,QDBusConnection::systemBus()));
    }
}
