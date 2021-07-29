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
 

#ifndef KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H
#define KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H

#include <kiran-system-daemon/accounts-i.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AuthManagerPage;
}
QT_END_NAMESPACE

class UserInterface;
class BiometricItem;
class KiranSwitchButton;
typedef QList<QPair<QString, QString>> BiometricList;

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
    BiometricList getBiometricItemsFromUI(AccountsAuthMode mode);
    BiometricList getBiometricItemsFromBackend(AccountsAuthMode mode);

    BiometricItem *newBiometricItem(const QString &name, const QString &dataID);

private slots:
    void slotCheckAuthTypes(bool checked);
    void slotItemDeleteClicked();
    void slotAddBiometricsItem();
    void slotUserPropertyChanged(QString path,QString propertyName,QVariant value);
    void slotUserAuthItemChanged();

private:
    Ui::AuthManagerPage *ui;
    QString m_userObjPath;
    UserInterface *m_userInterface = nullptr;

    KiranSwitchButton *m_fingerAuthSwitch;
    KiranSwitchButton *m_faceAuthSwitch;
    KiranSwitchButton *m_passwdAuthSwitch;

    BiometricItem *m_addFingerItem = nullptr;
    BiometricItem *m_addFaceItem = nullptr;
};

#endif  //KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H
