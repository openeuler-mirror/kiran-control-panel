//
// Created by lxh on 2021/3/11.
//

#ifndef KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H
#define KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H

#include <QWidget>
#include <kiran-system-daemon/accounts_i.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class AuthManagerPage;
}
QT_END_NAMESPACE

class UserInterface;
class BiometricItem;
typedef QList<QPair<QString, QString>> BiometricList;

class AuthManagerPage : public QWidget
{
    Q_OBJECT
public:
    explicit AuthManagerPage (QWidget *parent = nullptr);
    ~AuthManagerPage () override;

    /* 设置当前用户，更新界面，连接信号槽 */
    void setCurrentUser (const QString &userObj);

signals:
    /* 返回按钮点击，向上发送请求返回 */
    void sigReturn ();

private:
    /* 初始化界面 */
    void initUI ();
    /* 更新界面信息 */
    void updateInfo ();
    /* 保存界面信息 */
    void save ();

    QString generateBiometricsItemName (AccountsAuthMode mode);
    BiometricList getBiometricItemsFromUI (AccountsAuthMode mode);
    BiometricList getBiometricItemsFromBackend (AccountsAuthMode mode);

    BiometricItem *newBiometricItem (const QString &name, const QString &dataID);

private slots:
    void slotCheckAuthTypes (int state);
    void slotItemDeleteClicked ();
    void slotAddBiometricsItem ();

private:
    Ui::AuthManagerPage *ui;
    QString m_userObjPath;
    UserInterface *m_userInterface = nullptr;

    BiometricItem *m_addFingerItem = nullptr;
    BiometricItem *m_addFaceItem = nullptr;
};

#endif //KIRAN_ACCOUNT_MANAGER_AUTH_MANAGER_PAGE_H
