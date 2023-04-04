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
#ifndef KIRANGROUPMANAGER_H
#define KIRANGROUPMANAGER_H

#include "kiranwidgets-qt5/kiran-titlebar-window.h"

#include <QThread>
#include <QWidget>

class QStackedWidget;
class KiranSidebarWidget;
class HardWorker;
class QListWidgetItem;
class MaskWidget;
class CreateGroupPage;
class GroupInfoPage;


enum StackWidgetPageEnum
{
    PAGE_CREATE_GROUP,
    PAGE_GROUP_INFO
};

class KiranGroupManager : public QWidget
{
    Q_OBJECT
public:
    explicit KiranGroupManager(QWidget *parent = nullptr);
    ~KiranGroupManager();
    QSize sizeHint() const;
private Q_SLOTS:
    void appendSidebarItem(QString groupPath, QString errMsg);
    void deleteSidebarItem(QString groupName, QString errMsg);
    void updateSidebarItem(QString groupPath, QString errMsg);

private:
    void initUI();
    void initGroupList();
    void initPageCreateGroup();
    void initPageGroupInfo();
    //判断是否为用户自己创建的用户组（gid>=1000)
    bool isNoSystemGroup(const QString &groupPath);
    void setDefaultSiderbarItem();

private:
    KiranSidebarWidget *m_tabList = nullptr;
    QThread m_workThread;
    HardWorker *m_hardworker = nullptr;
    QListWidgetItem *m_createGroupItem = nullptr;
    CreateGroupPage *m_pageCreateGroup = nullptr;
    GroupInfoPage *m_page_groupInfo = nullptr;
    QStackedWidget *m_stackWidget = nullptr;
    QString m_currentItemData;
};

#endif  // KIRANGROUPMANAGER_H
