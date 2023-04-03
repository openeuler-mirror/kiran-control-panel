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
#include "hard-worker.h"
#include "config.h"
#include "groups-global-info.h"
#include "ksd_group_admin_list_proxy.h"
#include "ksd_group_admin_proxy.h"

#include <qt5-log-i.h>
#include <QDBusObjectPath>

HardWorker::HardWorker() : QObject(nullptr)
{
}

HardWorker::~HardWorker()
{
}

void HardWorker::doCreateGroup(QString groupName)
{
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();
    KSDGroupAdminProxy groupAdmin(GROUP_ADMIN_DBUS_NAME,
                                  GROUP_ADMIN_OBJECT_PATH,
                                  QDBusConnection::systemBus());
    QString groupObjPath;
    QString errMsgPrefix = tr("Create Group failed");
    QString errMsgDetail;

    QDBusPendingReply<QDBusObjectPath> createGroupRep;
    createGroupRep = groupAdmin.CreateGroup(groupName);
    createGroupRep.waitForFinished();
    if (createGroupRep.isError())
    {
        KLOG_WARNING() << "create group failed," << createGroupRep.error();
        errMsgDetail = createGroupRep.error().message();
        goto failed;
    }

    groupObjPath = createGroupRep.value().path();

    KLOG_INFO() << QString("create group(%1) is done").arg(groupName);
    emit sigCreateGroupDone(groupObjPath, "");
    return;
failed:
    if (!groupObjPath.isEmpty())
    {
        KSDGroupAdminListProxy groupListInterface(GROUP_ADMIN_DBUS_NAME,
                                                  groupObjPath,
                                                  QDBusConnection::systemBus());
        qulonglong groupID = groupListInterface.gid();
        auto reply = groupAdmin.DeleteGroup(groupID);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "create group failed,delete group:" << reply.error();
        }
    }
    QString errMsg = errMsgPrefix;
    if (!errMsgDetail.isEmpty())
    {
        errMsg.append(",");
        errMsg.append(errMsgDetail);
    }
    emit sigCreateGroupDone("", errMsg);
}

void HardWorker::doDeleteGroup(int gid, QString groupName)
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
        KLOG_INFO() << "delete group" << reply.error();
        QString errMsg = QString(tr("Failed to delete group,%1")).arg(reply.error().message());
        emit sigDeleteGroupDone("", errMsg);
    }
    else
    {
        emit sigDeleteGroupDone(groupName, "");
    }
}

void HardWorker::doAddUserToGroup(QString groupPath, QStringList userNameList)
{
    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME, groupPath,
                                      QDBusConnection::systemBus());

    for (auto &userName : userNameList)
    {
        auto reply = groupProxy.AddUserToGroup(userName);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "add user " << userName << " failed, " << reply.error();
            emit sigAddUserToGroupDone(tr(" add user to group failed"));
            return;
        }
        else
        {
            KLOG_INFO() << "add user " << userName << " to group is done";
        }
    }

    emit sigAddUserToGroupDone("");
}

void HardWorker::doRemoveMemberFromGroup(QString groupPath, QString userName)
{
    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME, groupPath,
                                      QDBusConnection::systemBus());

    auto reply = groupProxy.RemoveUserFromGroup(userName);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "remove member " << userName << " failed, " << reply.error();
        emit sigRemoveMemberFromGroupDone(tr(" add user to group failed"));
    }
    else
    {
        KLOG_INFO() << "remove member " << userName << " from group is done";
        emit sigRemoveMemberFromGroupDone("");
    }
}

void HardWorker::doChangeGroupName(QString groupPath, QString groupName)
{
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();

    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME, groupPath,
                                      QDBusConnection::systemBus());

    if (GroupsGlobalInfo::instance()->checkGroupNameAvaliable(groupName))
    {
        auto reply = groupProxy.ChangeGroupName(groupName);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "change group name for " << groupProxy.groupName() << "( to name " << groupName << ") failed, " << reply.error();
            emit sigChangeGroupNameDone("", tr(" change group name failed"));
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
        emit sigChangeGroupNameDone("", tr(" change group name failed, the new group name is occupied"));
    }
}
