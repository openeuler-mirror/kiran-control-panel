/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "group-name-checker.h"
#include "ksd_group_admin_proxy.h"

#include <kiran-system-daemon/groups-i.h>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QRegularExpression>

bool GroupNameChecker::isValid(const QString& name, QString& errorMessage)
{
    errorMessage = "";
    // 检验用户组名是否为空
    if (name.isEmpty())
    {
        errorMessage = tr("Please enter your group name");
        return false;
    }

    // 检验用户组名是否为纯数字
    QRegularExpression re("^[0-9]+$");
    bool isPureDigital = re.match(name).hasMatch();
    if (isPureDigital)
    {
        errorMessage = tr("Group name cannot be a pure number");
        return false;
    }

    // 检验用户组名是否为重复：直接问后端这个名字在不在，
    // 不再遍历本地所有用户组逐个取属性（用户组多的时候点"创建"会把界面卡住）
    KSDGroupAdminProxy groupAdminProxy(GROUPS_DBUS_NAME, GROUPS_OBJECT_PATH, QDBusConnection::systemBus());
    QDBusPendingReply<QDBusObjectPath> findGroupReply = groupAdminProxy.FindGroupByName(name);
    findGroupReply.waitForFinished();
    if (!findGroupReply.isError())
    {
        // 后端用错误回复表示没有发现该用户组
        errorMessage = tr("Group name already exists");
        return false;
    }

    return true;
}