/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-group is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     wangshichang <shichang@isrc.iscas.ac.cn>
 */
#include "group-interface.h"
#include "group-manager.h"
#include "ksd_group_admin_list_proxy.h"
#include "ksd_group_admin_proxy.h"

#include <qt5-log-i.h>
#include <QDBusObjectPath>

GroupInterface::GroupInterface() : QObject(nullptr)
{
}

GroupInterface::~GroupInterface()
{
}

void GroupInterface::doCreateGroup(QString groupName)
{
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();
    KSDGroupAdminProxy groupAdmin(GROUP_ADMIN_DBUS_NAME,
                                  GROUP_ADMIN_OBJECT_PATH,
                                  QDBusConnection::systemBus());
    QString groupObjPath;
    QString errMsg;

    QDBusPendingReply<QDBusObjectPath> reply;
    reply = groupAdmin.CreateGroup(groupName);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "create group failed," << reply.error();
        errMsg = QString(tr("Failed to create group, %1")).arg(reply.error().message());
        emit sigCreateGroupDone("", errMsg);
        return;
    }

    groupObjPath = reply.value().path();

    KLOG_INFO() << QString("create group(%1) is done.").arg(groupName);
    emit sigCreateGroupDone(groupObjPath, "");
}

void GroupInterface::doDeleteGroup(int gid, QString groupName)
{
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();

    KSDGroupAdminProxy groupAdmin(GROUP_ADMIN_DBUS_NAME,
                                  GROUP_ADMIN_OBJECT_PATH,
                                  QDBusConnection::systemBus());
    // 删除用户
    auto reply = groupAdmin.DeleteGroup(gid);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "delete group failed," << reply.error();
        QString errMsg = QString(tr("Failed to delete group, %1")).arg(reply.error().message());
        emit sigDeleteGroupDone("", errMsg);
    }
    else
    {
        KLOG_INFO() << QString("delete group(%1) is done.").arg(groupName);
        emit sigDeleteGroupDone(groupName, "");
    }
}

void GroupInterface::doAddUserToGroup(QString groupPath, QStringList userNameList)
{
    QString errMsg;
    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME, groupPath,
                                      QDBusConnection::systemBus());

    for (auto &userName : userNameList)
    {
        auto reply = groupProxy.AddUserToGroup(userName);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "add user " << userName << " failed, " << reply.error();
            errMsg = QString(tr("Failed to add %1 to group, %2")).arg(userName).arg(reply.error().message());
            break;
        }
        else
        {
            KLOG_INFO() << "add user " << userName << " to group is done";
        }
    }

    emit sigAddUserToGroupDone(errMsg);
}

void GroupInterface::doRemoveMemberFromGroup(QString groupPath, QString userName)
{
    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME, groupPath,
                                      QDBusConnection::systemBus());

    auto reply = groupProxy.RemoveUserFromGroup(userName);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "remove member " << userName << " failed, " << reply.error();
        auto errMsg = QString(tr("Failed to remove %1 from group, %2")).arg(userName).arg(reply.error().message());
        emit sigRemoveMemberFromGroupDone(errMsg);
    }
    else
    {
        KLOG_INFO() << "remove member " << userName << " from group is done";
        emit sigRemoveMemberFromGroupDone("");
    }
}

void GroupInterface::doChangeGroupName(QString groupPath, QString groupName)
{
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();

    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME, groupPath,
                                      QDBusConnection::systemBus());

    if (GroupManager::instance()->checkGroupNameAvaliable(groupName))
    {
        auto reply = groupProxy.ChangeGroupName(groupName);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "change group name for " << groupProxy.groupName() << "( to name " << groupName << ") failed, " << reply.error();
            auto errMsg = QString(tr("Failed to change group name to %1, %2")).arg(groupName).arg(reply.error().message());
            emit sigChangeGroupNameDone("", errMsg);
        }
        else
        {
            KLOG_INFO() << "change group name for " << groupProxy.groupName() << "( to name " << groupName << "is done";
            emit sigChangeGroupNameDone(groupPath, "");
        }
    }
    else
    {
        KLOG_WARNING() << "change group name for " << groupProxy.groupName() << "( to name " << groupName << ") failed, the new group name is occupied";
        auto errMsg = QString(tr("Failed to change group name to %1, the new group name is occupied!")).arg(groupName);
        emit sigChangeGroupNameDone("", errMsg);
    }
}
