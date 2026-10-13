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
#include "group-manager.h"
#include "group-loader.h"
#include "ksd_group_admin_list_proxy.h"
#include "ksd_group_admin_proxy.h"

#include <kiran-system-daemon/groups-i.h>
#include <qt5-log-i.h>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QThread>

GroupManager::GroupManager(QObject *parent)
    : QObject(parent),
      m_groupAdminProxy(new KSDGroupAdminProxy(GROUPS_DBUS_NAME, GROUPS_OBJECT_PATH, QDBusConnection::systemBus()))
{
    connect(m_groupAdminProxy, &KSDGroupAdminProxy::GroupAdded,
            this, &GroupManager::handleGroupAdded);
    connect(m_groupAdminProxy, &KSDGroupAdminProxy::GroupDeleted,
            this, &GroupManager::handleGroupDeleted);

    // 变化通知只在服务上订阅一次：空对象路径表示该服务下的所有对象，槽里再按消息里的
    // 对象路径区分是哪个用户组。不能每个用户组订阅一次：一个用户组一条 match rule，
    // 用户组数上千时会撞上 dbus-daemon 的规则数上限，超出的订阅会静默失效
    QDBusConnection::systemBus().connect(
        GROUPS_DBUS_NAME,
        QString(),
        QStringLiteral("com.kylinsec.Kiran.SystemDaemon.Groups.Group"),
        QStringLiteral("GroupChanged"),
        QStringLiteral("o"),
        this, SLOT(handleDbusGroupChanged(QDBusObjectPath)));
}

GroupManager::~GroupManager()
{
    stopBackgroundLoad();

    delete m_groupAdminProxy;
    m_groupAdminProxy = nullptr;
}

void GroupManager::loadGroups()
{
    // 获取所有用户组
    QDBusPendingReply<QList<QDBusObjectPath>> pendingReply = m_groupAdminProxy->ListCachedGroups();
    pendingReply.waitForFinished();
    if (pendingReply.isError())
    {
        KLOG_ERROR() << "GetCachedGroups Error:"
                     << pendingReply.error();
        return;
    }

    QStringList allPaths;
    const QList<QDBusObjectPath> objList = pendingReply.value();
    for (auto objListIter = objList.begin();
         objListIter != objList.end();
         ++objListIter)
    {
        allPaths.append(objListIter->path());
    }

    if (!allPaths.isEmpty())
    {
        KLOG_DEBUG() << "load" << allPaths.size() << "groups in background";
        startBackgroundLoad(allPaths);
    }
}

bool GroupManager::getGroupInfo(const QString &groupPath, GroupManager::GroupInfo &groupInfo)
{
    KSDGroupAdminListProxy interface(GROUPS_DBUS_NAME, groupPath, QDBusConnection::systemBus());
    if (!interface.isValid())
    {
        KLOG_WARNING() << "Failed to get group info:" << interface.lastError().message();
        return false;
    }
    KLOG_DEBUG() << "Get group info of" << groupPath << "from dbus:"
                 << "name:" << interface.name()
                 << "gid:" << interface.gid()
                 << "users:" << interface.users();

    groupInfo.name = interface.name();
    groupInfo.gid = interface.gid();
    groupInfo.users = interface.users();
    groupInfo.isNotSystemGroup = interface.gid() >= 1000 && interface.gid() < 65534;
    return true;
}

void GroupManager::handleGroupAdded(const QDBusObjectPath &group)
{
    // 新增用户组的名称、gid也放到加载器线程加载，不在主线程等D-Bus返回
    startBackgroundLoad(QStringList() << group.path());
}

void GroupManager::handleGroupDeleted(const QDBusObjectPath &group)
{
    KLOG_DEBUG() << "delete group:" << group.path();
    emit GroupDeleted(group.path());
}

void GroupManager::handleDbusGroupChanged(const QDBusObjectPath &group)
{
    KLOG_DEBUG() << "group changed:" << group.path();
    emit GroupChanged(group.path());
}

void GroupManager::handleGroupLoaded(const QString &groupPath, const QVariantMap &values)
{
    emitGroupAdded(groupPath, values);
}

void GroupManager::emitGroupAdded(const QString &groupPath, const QVariantMap &values)
{
    emit GroupAdded(groupPath,
                    values.value(QStringLiteral("name")).toString(),
                    values.value(QStringLiteral("gid")).toLongLong());
}

void GroupManager::startBackgroundLoad(const QStringList &objectPaths)
{
    if (objectPaths.isEmpty())
    {
        return;
    }

    if (!m_loaderThread)
    {
        m_loaderThread = new QThread(this);
        m_loader = new GroupLoader;
        m_loader->moveToThread(m_loaderThread);

        // 线程结束时由 loader 自身线程删除，避免主线程跨线程 delete
        connect(m_loaderThread, &QThread::finished, m_loader, &QObject::deleteLater);
        connect(m_loader, &GroupLoader::groupLoaded,
                this, &GroupManager::handleGroupLoaded);

        m_loaderThread->start();
    }

    // 队列连接，实际加载在加载器线程里做，加载完一个回调一个
    QMetaObject::invokeMethod(m_loader, "load", Qt::QueuedConnection, Q_ARG(QStringList, objectPaths));
}

void GroupManager::stopBackgroundLoad()
{
    if (!m_loaderThread)
    {
        return;
    }

    m_loader->requestStop();
    m_loaderThread->quit();

    if (!m_loaderThread->wait(10000))
    {
        KLOG_WARNING() << "group loader did not stop in 10s, detach it";
        // 超时：断开业务连接并放弃管理，避免崩溃
        disconnect(m_loader, nullptr, this, nullptr);
        m_loaderThread->setParent(nullptr);
        m_loader = nullptr;
        m_loaderThread = nullptr;
        return;
    }

    // m_loader 已随线程结束被删除，这里只删线程对象
    m_loader = nullptr;
    delete m_loaderThread;
    m_loaderThread = nullptr;
}
