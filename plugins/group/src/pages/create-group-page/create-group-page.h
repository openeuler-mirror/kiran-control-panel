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
class AccountsDataProvider;
class CreateGroupPage : public QWidget
{
    Q_OBJECT

public:
    CreateGroupPage(AccountsDataProvider *accountsDataProvider, QWidget *parent = nullptr);
    ~CreateGroupPage();

    void reset();

private:
    void initUI();
    void appendUserListItem(const QString &userName);
    void scheduleAppend();

private Q_SLOTS:
    void createGroup();
    void appendUser(const QString &objectPath, const QString &userName,
                    const QString &iconFile, bool locked);
    void appendPendingUsers();

public Q_SLOTS:
    void handleGroupAdded(const QString &groupPath, const QString &errMsg);

signals:
    void requestCreateGroup(const QString groupName, const QStringList &users);

private:
    Ui::CreateGroupPage *ui;
    KiranTips *m_errorTip;
    UsersContainer *m_userContainter;
    AccountsDataProvider *m_accountsDataProvider;
    QStringList m_allUserName;
    QStringList m_pendingUserNames;
    bool m_appendPending = false;
};
#endif  // CREATE GROUPPAGE_H
