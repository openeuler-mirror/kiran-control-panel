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

#pragma once

#include <QDBusObjectPath>
#include <QList>
#include <QObject>
#include <QStringList>
#include <QVariantMap>

class QThread;
class KSDGroupAdminProxy;
class GroupLoader;

/// @brief 用户组信息的持有者，生命周期跟着使用它的页面控件走
///        不为每个用户组创建 D-Bus 代理：名称与 gid 在后台线程加载，随 GroupAdded 一起下发；
///        变化通知在服务上订阅一次，槽里按消息里的对象路径区分用户组
class GroupManager : public QObject
{
    Q_OBJECT

public:
    struct GroupInfo
    {
        QString name;
        qlonglong gid;
        QStringList users;
        bool isNotSystemGroup = false;
    };

public:
    explicit GroupManager(QObject *parent = nullptr);
    ~GroupManager();

    /**
     * @brief 枚举用户组并在后台线程加载名称、gid，页面创建时调用一次
     */
    void loadGroups();

    /// @brief 获取用户组信息
    /// @param groupPath 用户组DBus对象路径
    /// @param groupInfo 存储用户组信息
    /// @return 是否获取成功
    bool getGroupInfo(const QString &groupPath, GroupManager::GroupInfo &groupInfo);

signals:
    void GroupAdded(const QString &groupPath, const QString &groupName, qlonglong gid);
    void GroupDeleted(const QString &groupPath);
    void GroupChanged(const QString &groupPath);

private Q_SLOTS:
    // 服务级订阅的回调，所有用户组的变化都从这里出来
    void handleDbusGroupChanged(const QDBusObjectPath &group);
    // 一个用户组加载完成
    void handleGroupLoaded(const QString &groupPath, const QVariantMap &values);

private:
    void handleGroupAdded(const QDBusObjectPath &group);
    void handleGroupDeleted(const QDBusObjectPath &group);
    void emitGroupAdded(const QString &groupPath, const QVariantMap &values);
    void startBackgroundLoad(const QStringList &objectPaths);
    void stopBackgroundLoad();

private:
    KSDGroupAdminProxy *m_groupAdminProxy = nullptr;
    GroupLoader *m_loader = nullptr;
    QThread *m_loaderThread = nullptr;
};
