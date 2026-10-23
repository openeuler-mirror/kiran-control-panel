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

#include <QHash>
#include <QThread>
#include <QWidget>

class QStackedWidget;
class KiranSidebarWidget;
class GroupInterface;
class GroupManager;
class AccountsDataProvider;
class QListWidgetItem;
class CreateGroupPage;
class GroupInfoPage;
class AddUsersPage;

class GroupPage : public QWidget
{
    Q_OBJECT

public:
    enum StackWidgetPageEnum
    {
        PAGE_CREATE_GROUP,
        PAGE_GROUP_INFO,
        PAGE_ADD_USERS
    };

public:
    explicit GroupPage(QWidget *parent = nullptr);
    ~GroupPage();
    QSize sizeHint() const;
    void jumpToPage(StackWidgetPageEnum page);

signals:
    /// 有用户新增：页面不直接连账号服务，由 GroupPage 统一转发
    /// @param userName 新增用户的用户名
    void userAdded(const QString &userName);

    /// 有用户被删除：页面据此清掉界面上的残留条目
    /// @param userName 被删除用户的用户名
    void userDeleted(const QString &userName);

private Q_SLOTS:
    void addGroup(const QString &groupPath, const QString &groupName, qlonglong gid);
    void deleteGroup(const QString &groupPath);
    void handleGroupChanged(const QString &groupPath);

    void deleteSidebarItem(const QString &groupPath);
    void updateSidebarItem(const QString &groupPath);

    /// 有用户新增：记下对象路径与用户名的对应关系后转发给页面。
    /// 记下来是因为删除信号只带对象路径，而那时用户对象已经被服务销毁、查不到名字了
    void handleUserAdded(const QString &objectPath, const QString &userName,
                         const QString &iconFile, bool locked);
    /// 用户被删除：把用户名转发给需要清理的页面
    void handleUserDeleted(const QString &objectPath);

private:
    void initUI();
    void initGroupList();
    void initPageCreateGroup();
    void initPageGroupInfo();
    void initPageAddUsers();
    void connectToInfoChange();

    void setDefaultSiderbarItem();

private:
    KiranSidebarWidget *m_tabList = nullptr;
    QThread m_workThread;
    GroupInterface *m_groupInterface = nullptr;
    GroupManager *m_groupManager = nullptr;
    AccountsDataProvider *m_accountsDataProvider = nullptr;
    QListWidgetItem *m_createGroupItem = nullptr;
    CreateGroupPage *m_pageCreateGroup = nullptr;
    GroupInfoPage *m_pageGroupInfo = nullptr;
    AddUsersPage *m_pageAddUsers = nullptr;
    QStackedWidget *m_stackWidget = nullptr;
    // 用户对象路径 -> 用户名：删除信号只给对象路径，删除时已经查不到名字，只能提前记下来
    QHash<QString, QString> m_userNameByPath;
};
