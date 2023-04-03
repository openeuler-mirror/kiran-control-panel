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

#include <QThread>
#include <QWidget>

#include "kiran-tips.h"
#include "kiranwidgets-qt5/kiran-sidebar-widget.h"
#include "users-container.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class CreateGroupPage;
}
QT_END_NAMESPACE

class CreateGroupPage : public QWidget
{
    friend class KiranGroupManager;
    Q_OBJECT

public:
    CreateGroupPage(QWidget *parent = nullptr);
    ~CreateGroupPage();

    void reset();

signals:
    void sigCreateGroup(QString groupName);
    void sigIsBusyChanged(bool isBusy);
    void sigRequestSetCurrentGroup(const QString &groupPath);
    void sigAddUserToGroup(QString groupPath, QStringList userName);

private:
    void initUI();
    void initUserList();
    void appendUserListItem(const QString &userPath);
    void loadUsers();

public Q_SLOTS:
    void handlerCreateNewGroup();
    void handlerCreateNewGroupIsDone(QString groupPath, QString errMsg);
    void handlerAddUserToGroupIsDone(QString errMsg);

private:
    Ui::CreateGroupPage *ui;
    KiranTips *m_errorTip;
    UsersContainer *m_userContainter;
};
#endif  // CREATE GROUPPAGE_H
