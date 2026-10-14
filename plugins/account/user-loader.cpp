/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     gaobo <gaobo@kylinsec.com.cn>
 */
#include "user-loader.h"
#include "kiran-account-service-wrapper.h"
#include "logging-category.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <algorithm>
#include <qt5-log-i.h>

const int FetchValuesTimeoutMs = 2000;

UserLoader::UserLoader()
{
}

QVariantMap UserLoader::fetchUserValues(const QString &objectPath)
{
    QDBusMessage request = QDBusMessage::createMethodCall(DBusWrapper::KiranAccountServiceName,
                                                          objectPath,
                                                          QStringLiteral("org.freedesktop.DBus.Properties"),
                                                          QStringLiteral("GetAll"));
    request << QString::fromLatin1(KiranAccountServiceUser::staticInterfaceName());

    const QDBusMessage reply = QDBusConnection::systemBus().call(request, QDBus::Block, FetchValuesTimeoutMs);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty())
    {
        return QVariantMap();
    }

    const QVariant &value = reply.arguments().at(0);
    if (value.userType() == qMetaTypeId<QDBusArgument>())
    {
        return qdbus_cast<QVariantMap>(value.value<QDBusArgument>());
    }
    return value.toMap();
}

void UserLoader::requestStop()
{
    m_stopRequested.storeRelease(1);
}

void UserLoader::load(const QStringList &objectPaths)
{
    // 与旧实现下发的顺序保持一致：按对象路径排序
    QStringList paths = objectPaths;
    std::sort(paths.begin(), paths.end());

    for (const QString &objectPath : paths)
    {
        if (m_stopRequested.loadAcquire() != 0)
        {
            KLOG_DEBUG(qLcAccount) << "user loader stopped," << paths.size() << "users not loaded";
            break;
        }

        const QVariantMap values = fetchUserValues(objectPath);
        if (values.isEmpty())
        {
            // 读取失败，或者读取的途中该用户已经被删除
            KLOG_WARNING(qLcAccount) << "fetch user values failed:" << objectPath;
            continue;
        }

        emit userLoaded(objectPath, values);
    }
}
