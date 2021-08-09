/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-account is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

 
#ifndef KIRANACCOUNTMANAGER_H
#define KIRANACCOUNTMANAGER_H

#include "kiranwidgets-qt5/kiran-titlebar-window.h"

#include <QThread>
#include <QWidget>

class QStackedWidget;
class CreateUserPage;
class UserInfoPage;
class SelectAvatarPage;
class AuthManagerPage;
class MaskWidget;
class HardWorker;
class KiranSidebarWidget;
class QListWidgetItem;
class KiranAccountManager : public QWidget
{
    Q_OBJECT
public:
    explicit KiranAccountManager(QWidget *parent = nullptr);
    ~KiranAccountManager();

    virtual QSize sizeHint() const;

private Q_SLOTS:
    void setCurrentUser(const QString &userPath);
    void setMaskVisible(bool visible);

private:
    void initUI();
    void initUserList();
    void initPageCreateUser();
    void initPageUserInfo();
    void initPageSelectAvatar();
    void initPageAuthManager();
    void connectToInfoChanged();
    void appendSiderbarItem(const QString &userPath);
    void setDefaultSiderbarItem();

private:
    KiranSidebarWidget *m_tabList{};
    QListWidgetItem *m_createUserItem{};
    QStackedWidget *m_stackWidget{};
    CreateUserPage *m_page_createUser{};
    UserInfoPage *m_page_userinfo{};
    SelectAvatarPage *m_page_selectAvatar{};
    AuthManagerPage *m_page_authManager{};
    MaskWidget *m_maskWidget{};
    QThread m_workThread;
    HardWorker *m_hardworker;
};

#endif  // KIRANACCOUNTMANAGER_H
