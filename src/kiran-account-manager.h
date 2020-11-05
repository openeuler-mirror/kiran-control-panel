#ifndef KIRANACCOUNTMANAGER_H
#define KIRANACCOUNTMANAGER_H

#include "kiranwidgets-qt5/kiran-titlebar-window.h"

#include <QWidget>
#include <QThread>

class ListWidgetControl;
class QStackedWidget;
class CreateUserPage;
class UserInfoPage;
class SelectAvatarPage;
class MaskWidget;
class HardWorker;

class KiranAccountManager : public KiranTitlebarWindow {
Q_OBJECT
public:
    KiranAccountManager();
    ~KiranAccountManager();

private Q_SLOTS:
    void setCurrentUser(const QString &userPath);

private:
    void initUI();
    void initUserList();
    void initPageCreateUser();
    void initPageUserInfo();
    void initPageSelectAvatar();
    void connectToInfoChanged();
    void appendSiderbarItem(const QString &userPath);
    void setDefaultSiderbarItem();

    void showMask();
    void hideMask();
private:
    ListWidgetControl *m_tabList;
    QStackedWidget *m_stackWidget;
    CreateUserPage *m_page_createUser;
    UserInfoPage *m_page_userinfo;
    SelectAvatarPage *m_page_selectAvatar;
    MaskWidget *m_maskWidget;
    QThread m_workThread;
    HardWorker *m_hardworker;
};

#endif // KIRANACCOUNTMANAGER_H
