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

 

#ifndef KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H
#define KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H

#include <kiran-system-daemon/accounts-i.h>
#include <QWidget>
#include "json-parser.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class AuthManagerPage;
}
QT_END_NAMESPACE

class KSDAccountsUserProxy;
class BiometricItem;
class KiranSwitchButton;
class AuthManagerPage : public QWidget
{
    Q_OBJECT
public:
    explicit AuthManagerPage(QWidget *parent = nullptr);
    ~AuthManagerPage() override;

    /* 设置当前用户，更新界面，连接信号槽 */
    void setCurrentUser(const QString &userObj);

signals:
    /* 返回按钮点击，向上发送请求返回 */
    void sigReturn();

private:
    /* 初始化界面 */
    void initUI();
    /* 更新界面信息 */
    void updateInfo();
    /* 保存界面信息 */
    void save();

    QString generateBiometricsItemName(AccountsAuthMode mode);
    BiometricInfos getBiometricInfoFromUI(AccountsAuthMode mode);
    BiometricInfos getBiometricInfoFromBackend(AccountsAuthMode mode);

    BiometricItem *newBiometricItem(const QString &name, const QString &dataID);

private slots:
    void slotCheckAuthTypes(bool checked);
    void slotItemDeleteClicked();
    void slotAddBiometricsItem();
    void slotUserPropertyChanged(QString propertyName, QVariant value);
    void slotUserAuthItemChanged();

private:
    Ui::AuthManagerPage *ui;
    QString m_userObjPath;
    KSDAccountsUserProxy *m_userProxy = nullptr;

    KiranSwitchButton *m_fingerAuthSwitch;
    KiranSwitchButton *m_faceAuthSwitch;
    KiranSwitchButton *m_passwdAuthSwitch;

    BiometricItem *m_addFingerItem = nullptr;
    BiometricItem *m_addFaceItem = nullptr;
};

#endif  //KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H
