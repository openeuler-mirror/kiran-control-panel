 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd. 
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
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
class PasswordExpirationPolicyPage;
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
    void initPagePasswdExpirationPolicy();
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
    PasswordExpirationPolicyPage *m_page_passwdExpirationPolicy{};
    MaskWidget *m_maskWidget{};
    QThread m_workThread;
    HardWorker *m_hardworker;
};

#endif  // KIRANACCOUNTMANAGER_H
