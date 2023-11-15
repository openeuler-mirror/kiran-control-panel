/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once
#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QThread>
#include <QWidget>

class QStackedWidget;
class CreateUserPage;
class UserInfoPage;
class SelectAvatarPage;
class PasswordExpirationPolicyPage;
class MaskWidget;
class HardWorker;
class KiranSidebarWidget;
class QListWidgetItem;

class AccountWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccountWidget(QWidget *parent = nullptr);
    ~AccountWidget();
    void jumpToUser(const QString &user);
    void jumpToAddUser();
    QSize sizeHint() const override;

private:
    void initUI();
    void initUserList();
    void initPageCreateUser();
    void initPageUserInfo();
    void initPageSelectAvatar();
    void initPagePasswdExpirationPolicy();
    void connectToInfoChange();

private Q_SLOTS:
    void setCurrentUser(const QString &userPath);
    void setMaskVisible(bool visible);
    void updateCreateUserIcon();
    void onUserAdded(const QString& objectPath);
    void onUserDeleted(const QString& objectPath);
    void onUserPropertyChanged(const QString& objectPath,const QString& propertyName,QVariant value);
    void onRequestIconPageForNewUser(const QString& iconPath);
    void onRequestSetCurrentUser(const QString& userPath);

private:
    void appendUser(const QString &userPath);
    void setDefaultSelectedUser();

private:
    KiranSidebarWidget *m_tabList = nullptr;
    QListWidgetItem *m_createUserItem = nullptr;
    QStackedWidget *m_stackWidget = nullptr;
    CreateUserPage *m_page_createUser = nullptr;
    UserInfoPage *m_page_userinfo = nullptr;
    SelectAvatarPage *m_page_selectAvatar = nullptr;
    PasswordExpirationPolicyPage *m_page_passwdExpirationPolicy = nullptr;
    MaskWidget *m_maskWidget = nullptr;
    QThread m_workThread;
    HardWorker *m_hardworker = nullptr;
};
