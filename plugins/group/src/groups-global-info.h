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
#ifndef GROUPSGLOBALINFO_H
#define GROUPSGLOBALINFO_H

#include "ksd_group_admin_proxy.h"
#include "ksd_group_admin_list_proxy.h"

#include <QList>
#include <QObject>

class GroupsGlobalInfo : public QObject
{
    Q_OBJECT
private:
    explicit GroupsGlobalInfo(QObject *parent = nullptr);

public:
    ~GroupsGlobalInfo();

    static GroupsGlobalInfo *instance();

    /**
     * @brief 初始化，加载用户组列表
     * @return
     */
    bool init();

    /**
     * @brief  获取排序之后的用户组列表
     * @return QList<QString> 用户DBusObjectPath列表
     */
    QList<QString> getGroupList();

    /**
     * @brief 检查是否存在重名用户组
     * @param account 需检查的用户组名
     * @return 是否可用
     */
    bool checkGroupNameAvaliable(const QString &groupName);

private:
    void addGroupToMap(const QDBusObjectPath &group);
    void deleteGroupFromMap(const QDBusObjectPath &group);

signals:
    void GroupAdded(const QDBusObjectPath &obj);
    void GroupDeleted(const QDBusObjectPath &obj);
    void GroupPropertyChanged(QString groupPath,
                              QString propertyName,
                              QVariant value);

private Q_SLOTS:
    void handlerPropertyChanged(const QString &propertyName, const QVariant &value);

private:
    KSDGroupAdminProxy m_groupAdminInterface;
    QMap<QString,KSDGroupAdminListProxy*> m_groupsMap; // QMap<DBus对象路径,用户相关接口>
};

#endif  // GROUPSGLOBALINFO_H