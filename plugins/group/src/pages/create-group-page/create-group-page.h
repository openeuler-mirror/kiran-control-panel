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
#ifndef CREATEGROUPPAGE_H
#define CREATEGROUPPAGE_H

#include <QDBusObjectPath>
#include <QWidget>

namespace Ui
{
class CreateGroupPage;
}

class UsersContainer;
class KiranTips;
class CreateGroupPage : public QWidget
{
    Q_OBJECT

public:
    CreateGroupPage(QWidget *parent = nullptr);
    ~CreateGroupPage();

    void reset();

private:
    void initUI();
    void appendUserListItem(const QString &userName);
    void scheduleAppend();

private Q_SLOTS:
    void createGroup();
    void appendPendingUsers();

public Q_SLOTS:
    /// 有用户新增时追加到成员候选列表（信号来自 GroupPage 的转发）
    /// @param userName 新增用户的用户名
    void appendUser(const QString &userName);

    /// 用户被删除时把成员候选列表里的条目清掉，避免已删除的用户还留在界面上
    /// @param userName 被删除用户的用户名（由 GroupPage 从对象路径解析后传入）
    void removeUser(const QString &userName);

    void handleGroupAdded(const QString &groupPath, const QString &errMsg);

signals:
    void requestCreateGroup(const QString groupName, const QStringList &users);

private:
    Ui::CreateGroupPage *ui;
    KiranTips *m_errorTip;
    UsersContainer *m_userContainter;
    QStringList m_allUserName;
    QStringList m_pendingUserNames;
    bool m_appendPending = false;
};
#endif  // CREATE GROUPPAGE_H
