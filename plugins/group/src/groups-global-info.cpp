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
#include "groups-global-info.h"
#include "def.h"
#include "ksd_group_admin_list_proxy.h"
#include "ksd_group_admin_proxy.h"

#include <qt5-log-i.h>
#include <QDBusObjectPath>

GroupsGlobalInfo::GroupsGlobalInfo(QObject *parent)
    : QObject(parent),
      m_groupAdminInterface(GROUP_ADMIN_DBUS_NAME, GROUP_ADMIN_OBJECT_PATH, QDBusConnection::systemBus())
{
}

GroupsGlobalInfo::~GroupsGlobalInfo()
{
}

GroupsGlobalInfo *GroupsGlobalInfo::instance()
{
    static QMutex mutex;
    static QScopedPointer<GroupsGlobalInfo> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new GroupsGlobalInfo);
        }
    }

    return pInst.data();
}

bool GroupsGlobalInfo::init()
{
    connect(&m_groupAdminInterface, &KSDGroupAdminProxy::GroupAdded, [this](const QDBusObjectPath &group)
            { addGroupToMap(group); });
    connect(&m_groupAdminInterface, &KSDGroupAdminProxy::GroupDeleted, [this](const QDBusObjectPath &group)
            { deleteGroupFromMap(group); });

    QList<QDBusObjectPath> groups;
    QDBusPendingReply<QList<QDBusObjectPath>> pendingReply;
    QList<QDBusObjectPath> objList;
    QList<QDBusObjectPath>::iterator objListIter;

    // 获取所有用户组
    pendingReply = m_groupAdminInterface.ListCachedGroups();
    pendingReply.waitForFinished();
    if (pendingReply.isError())
    {
        KLOG_ERROR() << "GetCachedGroups Error:"
                     << pendingReply.error();
        return false;
    }

    objList = pendingReply.value();
    // 将用户组列表添加到表中
    for (objListIter = objList.begin();
         objListIter != objList.end();
         ++objListIter)
    {
        addGroupToMap(*objListIter);
    }
    return true;
}

QList<QString> GroupsGlobalInfo::getGroupList()
{
    QList<QString> groupObjPathList;
    for (auto iter = m_groupsMap.begin();
         iter != m_groupsMap.end();
         iter++)
    {
        groupObjPathList.append((*iter)->path());
    }
    return groupObjPathList;
}

bool GroupsGlobalInfo::checkGroupNameAvaliable(const QString &groupName)
{
    bool isValid = true;

    for (auto &iter : m_groupsMap)
    {
        if (iter->groupName() == groupName)  // KSDGroupAdminListProxy的成员函数groupName()
        {
            isValid = false;
            break;
        }
    }

    return isValid;
}

void GroupsGlobalInfo::addGroupToMap(const QDBusObjectPath &group)
{
    if (m_groupsMap.find(group.path()) != m_groupsMap.end())
    {
        return;
    }

    auto groupProxy = new KSDGroupAdminListProxy(GROUP_ADMIN_DBUS_NAME,
                                                 group.path(),
                                                 QDBusConnection::systemBus(),
                                                 this);

    connect(groupProxy,
            &KSDGroupAdminListProxy::dbusPropertyChanged,
            this,
            &GroupsGlobalInfo::handlerPropertyChanged);

    m_groupsMap.insert(group.path(), groupProxy);

    emit GroupAdded(group.path());
}

void GroupsGlobalInfo::deleteGroupFromMap(const QDBusObjectPath &group)
{
    if (m_groupsMap.find(group.path()) == m_groupsMap.end())
    {
        return;
    }

    auto groupProxy = m_groupsMap.take(group.path());
    disconnect(groupProxy,
               &KSDGroupAdminListProxy::dbusPropertyChanged,
               this,
               &GroupsGlobalInfo::handlerPropertyChanged);
    delete groupProxy;

    emit GroupDeleted(group.path());
}

void GroupsGlobalInfo::handlerPropertyChanged(const QString &propertyName, const QVariant &value)
{
    auto groupProxy = qobject_cast<KSDGroupAdminListProxy *>(sender());

    KLOG_DEBUG() << "property changed:" << groupProxy->path();
    KLOG_DEBUG() << "\tname: " << propertyName;
    KLOG_DEBUG() << "\tvalue:" << value;

    emit GroupPropertyChanged(groupProxy->path(), propertyName, value);
}
