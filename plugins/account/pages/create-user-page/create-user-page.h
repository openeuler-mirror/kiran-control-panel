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

#ifndef CREATEUSERPAGE_H
#define CREATEUSERPAGE_H

#include <QThread>
#include <QWidget>

#include "advance-settings-page/advance-settings.h"

class KiranTips;

namespace Ui
{
class CreateUserPage;
}

class CreateUserPage : public QWidget
{
    friend class AccountWidget;
    Q_OBJECT
public:
    explicit CreateUserPage(QWidget *parent = nullptr);
    ~CreateUserPage();
    void reset();
    void setAvatarIconPath(const QString &iconPath);
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void requestCreateUser(QString userName,
                       int uid,
                       int userType,
                       QString encryptedPasswd,
                       QString homeDir,
                       QString shell,
                       QString iconFile);
    void requestIconPageForNewUser(QString iconPath);
    void requestSetCurrentUser(const QString &userPath);
    void busyChanged(bool isBusy);

private:
    void initUI();

private Q_SLOTS:
    void onCreateUserClicked();
    void onCreateUserDone(QString userPath, QString errMsg);

private:
    Ui::CreateUserPage *ui;
    AdvanceSettingsInfo m_advanceSettingsInfo;
    KiranTips *m_errorTip;
};

#endif  // CREATEUSERPAGE_H
