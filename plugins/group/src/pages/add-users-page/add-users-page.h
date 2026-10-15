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

#ifndef ADDUSERSPAGE_H
#define ADDUSERSPAGE_H

#include <QDBusObjectPath>
#include <QWidget>

namespace Ui
{
class AddUsersPage;
}

class UsersContainer;
class AccountsDataProvider;
class GroupManager;
class AddUsersPage : public QWidget
{
    Q_OBJECT

public:
    AddUsersPage(AccountsDataProvider *accountsDataProvider, GroupManager *groupManager, QWidget *parent = nullptr);
    ~AddUsersPage();

    void updateUsersList(const QString &groupObj);

signals:
    void requestGroupInfoPage();

    /// 向用户组添加成员的信号
    /// \param groupPath 要添加成员的用户组DBus对象路径
    /// \param userName 要添加进用户组的成员名
    void requestAddUserToGroup(const QString &groupPath, const QStringList &userName);

public Q_SLOTS:
    void searchFilter(const QString &filterString);

    void updateUI(const QString &errMsg);

    void appendUser(const QString &objectPath, const QString &userName,
                    const QString &iconFile, bool locked);

    void appendPendingUsers();

private:
    /// 初始化界面
    void initUI();
    void appendUserListItem(const QString &userName);
    void scheduleAppend();

private:
    Ui::AddUsersPage *ui;
    QStringList m_allUserName;
    QStringList m_usersInGroup;
    UsersContainer *m_usersContainer;
    AccountsDataProvider *m_accountsDataProvider;
    GroupManager *m_groupManager;
    QString m_curShowGroupPath;
    QStringList m_pendingUserNames;
    bool m_appendPending = false;
};
#endif  // ADDUSERSPAGE_H
