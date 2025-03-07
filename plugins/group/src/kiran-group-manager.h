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

#include <QThread>
#include <QWidget>

class QStackedWidget;
class KiranSidebarWidget;
class HardWorker;
class QListWidgetItem;
class CreateGroupPage;
class GroupInfoPage;
class AddUsersPage;

enum StackWidgetPageEnum
{
    PAGE_CREATE_GROUP,
    PAGE_GROUP_INFO,
    PAGE_ADD_USERS
};

class KiranGroupManager : public QWidget
{
    Q_OBJECT
public:
    explicit KiranGroupManager(QWidget *parent = nullptr);
    ~KiranGroupManager();
    QSize sizeHint() const;

private Q_SLOTS:
    void onGroupAdded(const QString &groupPath);
    void onGroupDeleted(const QString &groupPath);
    void onGroupPropertyChanged(QString groupPath,
                                QString propertyName,
                                QVariant value);

    void appendSidebarItem(const QString &groupPath);
    void deleteSidebarItem(const QString &groupName);
    void updateSidebarItem(QString groupPath, QString errMsg);

private:
    void initUI();
    void initGroupList();
    void initPageCreateGroup();
    void initPageGroupInfo();
    void initPageAddUsers();
    void connectToInfoChange();

    // 判断是否为用户自己创建的用户组（gid>=1000)
    bool isNoSystemGroup(qulonglong gid);
    void setDefaultSiderbarItem();

private:
    KiranSidebarWidget *m_tabList = nullptr;
    QThread m_workThread;
    HardWorker *m_hardworker = nullptr;
    QListWidgetItem *m_createGroupItem = nullptr;
    CreateGroupPage *m_pageCreateGroup = nullptr;
    GroupInfoPage *m_pageGroupInfo = nullptr;
    AddUsersPage *m_pageAddUsers = nullptr;
    QStackedWidget *m_stackWidget = nullptr;
};
