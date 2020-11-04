#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include "kiran-tips.h"

#include <QWidget>

namespace Ui {
    class UserInfoPage;
}

class UserInfoPage : public QWidget {
Q_OBJECT

public:
    explicit UserInfoPage(QWidget *parent = nullptr);

    ~UserInfoPage();

    void updateInfo();

    void setCurrentShowUserPath(const QString &userObj);
    QString getCurrentShowUserPath();
    QString getCurrentShowUserName();

    void setAvatarIconPath(const QString &iconPath);

signals:
    void sigUserChangeIcon(const QString &iconPath);

private:
    void initUI();
    void resetPageSetPasswd();

private Q_SLOTS:
    void handlerSaveUserProperty();
    void handlerSaveNewPasswd();

private:
    Ui::UserInfoPage *ui;
    QString m_curShowUserPath;
    QString m_curShowUserName;
    uid_t m_uid;
    KiranTips *m_errorTip;
};

#endif // USERINFOPAGE_H
