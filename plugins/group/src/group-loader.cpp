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
#include "group-loader.h"

#include <kiran-system-daemon/groups-i.h>
#include <qt5-log-i.h>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <algorithm>

namespace
{
const int FetchGroupValuesTimeoutMs = 2000;
}

GroupLoader::GroupLoader()
{
}

QVariantMap GroupLoader::fetchGroupValues(const QString &groupPath)
{
    QDBusMessage request = QDBusMessage::createMethodCall(GROUPS_DBUS_NAME,
                                                          groupPath,
                                                          QStringLiteral("org.freedesktop.DBus.Properties"),
                                                          QStringLiteral("GetAll"));
    request << QStringLiteral("com.kylinsec.Kiran.SystemDaemon.Groups.Group");

    const QDBusMessage reply = QDBusConnection::systemBus().call(request, QDBus::Block, FetchGroupValuesTimeoutMs);
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

void GroupLoader::requestStop()
{
    m_stopRequested.storeRelease(1);
}

void GroupLoader::load(const QStringList &groupPaths)
{
    // 与旧实现下发的顺序保持一致：按对象路径排序
    QStringList paths = groupPaths;
    std::sort(paths.begin(), paths.end());

    for (const QString &groupPath : paths)
    {
        if (m_stopRequested.loadAcquire() != 0)
        {
            KLOG_DEBUG() << "group loader stopped," << paths.size() << "groups not loaded";
            break;
        }

        const QVariantMap values = fetchGroupValues(groupPath);
        if (values.isEmpty())
        {
            // 读取失败，或者读取的途中该用户组已经被删除
            KLOG_WARNING() << "fetch group values failed:" << groupPath;
            continue;
        }

        emit groupLoaded(groupPath, values);
    }
}
