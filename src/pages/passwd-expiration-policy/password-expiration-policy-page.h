//
// Created by lxh on 2021/8/7.
//

#ifndef KIRAN_CPANEL_ACCOUNT_SRC_PAGES_PASSWD_EXPIRATION_POLICY_PASSWORD_EXPIRATION_POLICY_PAGE_H_
#define KIRAN_CPANEL_ACCOUNT_SRC_PAGES_PASSWD_EXPIRATION_POLICY_PASSWORD_EXPIRATION_POLICY_PAGE_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class PasswordExpirationPolicyPage;
}
QT_END_NAMESPACE

class KiranSwitchButton;
class UserInterface;
class PasswordExpirationPolicyPage : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordExpirationPolicyPage(QWidget *parent = nullptr);
    ~PasswordExpirationPolicyPage() override;

    /* 设置当前用户，更新界面，连接信号槽 */
    void setCurrentUser(const QString &userObj);

private:
    void initUI();
    void updateInfo();
    void save();
    
signals:
    void sigReturn();

private slots:
    void slotUserPropertyChanged(QString path,QString propertyName,QVariant value);

    void handleUserExpireSwitchToggled(bool checked);
    void handlePasswdMaxVaildDaysSwitchToggled(bool checked);
    void handlePromptTimeSwitchToggled(bool checked);
    void handleInactiveDaysSwitchToggled(bool checked);

private:
    Ui::PasswordExpirationPolicyPage *ui;
    UserInterface *m_userInterface = nullptr;
    KiranSwitchButton* m_userExpiresSwitch = nullptr;
    KiranSwitchButton* m_passwdMaxVaildDaysSwtich = nullptr;
    KiranSwitchButton* m_promptBeforePasswdExpirationSwitch = nullptr;
    KiranSwitchButton* m_passwdInactiveSwitch = nullptr;
};

#endif  //KIRAN_CPANEL_ACCOUNT_SRC_PAGES_PASSWD_EXPIRATION_POLICY_PASSWORD_EXPIRATION_POLICY_PAGE_H_
