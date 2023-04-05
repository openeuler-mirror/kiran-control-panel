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
#ifndef GROUPINFOPAGE_H
#define GROUPINFOPAGE_H

#include "kiran-tips.h"
#include "users-container.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class GroupInfoPage;
}
QT_END_NAMESPACE

class GroupInfoPage : public QWidget
{
    friend class KiranGroupManager;
    Q_OBJECT

public:
    GroupInfoPage(QWidget *parent = nullptr);
    ~GroupInfoPage();

    ///　从GroupAdmin服务中重新加载用户信息
    void updateInfo();

    /// 设置当前显示的用户组的DBus对象路径
    /// \param groupObj 用户组DBus对象路径
    void setCurrentShowGroupPath(const QString &groupObj);

    QString getGroupName() { return this->m_curShowGroupName; }

signals:
    /// 向HardWorker发送移除成员的信号
    /// \param groupPath 要移除成员的用户组DBus对象路径
    /// \param userName 要被移除的成员名
    void sigRemoveMember(QString groupPath, QString userName);

    /// 向HardWorker发送移除用户组的信号
    /// \param gid 要移除的用户组id
    void sigDeleteGroup(int gid, QString groupName);

    /// 向HardWorker发送更改用户组名称的信号
    /// \param groupPath 要更改名称的用户组DBus对象路径
    /// \param groupName 新的用户组名
    void sigChangeGroupName(QString groupPath, QString groupName);

    /// 向用户组添加成员的信号
    /// \param groupPath 要添加成员的用户组DBus对象路径
    /// \param userName 要添加进用户组的成员名
    void sigAddUserToGroup(QString groupPath, QStringList userName);

private Q_SLOTS:
    void handlerRemoveMemberIsDone(QString errMsg);

    void handlerDeleteGroupIsDone(QString groupPath, QString errMsg);

    void handlerChangeGroupNameIsDone(QString groupPath, QString errMsg);

    void handlerAddUserToGroupIsDone(QString errMsg);

    void searchFilter(QString filterString);

    void updateIcon();

private:
    /// 初始化界面
    void initUI();
    /// 初始化成员列表
    void initMemberList();

    bool eventFilter(QObject *watched, QEvent *event);

    void appendUserListItem(const QString &userPath);

    void appendMemberListItem(const QString &userName);

private:
    Ui::GroupInfoPage *ui;
    KiranTips *m_errorTip;
    QString m_curShowGroupPath;
    QString m_curShowGroupName;
    UsersContainer *m_memberContainer;
    UsersContainer *m_usersContainer;
    int gid;
};
#endif  // GROUPINFOPAGE_H
