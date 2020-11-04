#include "user-info-page.h"
#include "ui_user-info-page.h"
#include "accounts-user-interface.h"
#include "accounts-global-info.h"
#include "passwd-helper.h"

#include <QListView>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QMessageBox>

enum PageEnum {
    PAGE_USER_INFO,
    PAGE_CHANGE_PASSWD
};

UserInfoPage::UserInfoPage(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::UserInfoPage) {
    ui->setupUi(this);
    initUI();
}

UserInfoPage::~UserInfoPage() {
    delete ui;
}

void UserInfoPage::updateInfo() {
    m_errorTip->hideTip();

    UserInterface userInterface(m_curShowUserPath,
                                QDBusConnection::systemBus());

    QString userName = userInterface.user_name();
    m_uid = userInterface.uid();
    int accountType = userInterface.account_type();
    QString iconFile = userInterface.icon_file();
    bool locked = userInterface.locked();

    ui->label_name->setText(userName);
    ui->edit_userID->setText(QString::number(m_uid));
    ui->combo_accountType->setCurrentIndex(accountType);
    ui->avatar->setImage(iconFile);
    ui->checkBox->setChecked(!locked);
    m_curShowUserName = userName;

    if (m_curShowUserName != AccountsGlobalInfo::instance()->getCurrentUser()) {
        ui->passwd_row_1->setVisible(false);
        ui->passwd_row_2->setVisible(false);
    } else {
        ui->passwd_row_1->setVisible(true);
        ui->passwd_row_2->setVisible(true);
    }

    ui->stackedWidget->setCurrentIndex(PAGE_USER_INFO);
}

void UserInfoPage::setCurrentShowUserPath(const QString &userObj) {
    m_curShowUserPath = userObj;
    updateInfo();
}

QString UserInfoPage::getCurrentShowUserPath() {
    return m_curShowUserPath;
}

QString UserInfoPage::getCurrentShowUserName() {
    return m_curShowUserName;
}

void UserInfoPage::setAvatarIconPath(const QString &iconPath) {
    ui->avatar->setImage(iconPath);
}

void UserInfoPage::initUI() {
    m_errorTip = new KiranTips(this);
    m_errorTip->setShowPosition(KiranTips::POSITION_BOTTM);
    m_errorTip->setAnimationEnable(true);

    /* 用户显示页面 */
    //账户头像
    ui->avatar->setHoverImage(":/images/change_user_icon.png");
    ui->avatar->setClickEnable(true);
    connect(ui->avatar, &UserAvatarWidget::pressed, [this]() {
        emit sigUserChangeIcon(ui->avatar->iconPath());
    });

    //用户类型显示
    auto view = new QListView(ui->combo_accountType);
    ui->combo_accountType->setView(view);
    ui->combo_accountType->addItem(tr("standard"));
    ui->combo_accountType->addItem(tr("administrator"));
    ui->combo_accountType->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    //修改密码按钮
    connect(ui->btn_changePasswd, &QPushButton::clicked, [this]() {
        resetPageSetPasswd();
        ui->stackedWidget->setCurrentIndex(PAGE_CHANGE_PASSWD);
    });

    //确认按钮
    connect(ui->btn_confirm, &QPushButton::clicked,
            this, &UserInfoPage::handlerSaveUserProperty);

    //删除用户
    connect(ui->btn_deleteUser, &QPushButton::clicked, [this] {
        QString tip = QString(tr("The directory and files under the user's home directory are deleted with the user."
                                 "Are you sure you want to delete the user(%1)?")).arg(m_curShowUserName);
        KiranMessageBox::KiranStandardButton btn = KiranMessageBox::message(this,tr("Warning"),
                                  tip,
                                  KiranMessageBox::Yes|KiranMessageBox::No);

        if (btn == KiranMessageBox::Yes) {
            AccountsInterface accountInterface(QDBusConnection::systemBus());
            QDBusPendingReply<> reply = accountInterface.DeleteUser(m_uid, true);
            reply.waitForFinished();
            if (reply.isError()) {
                qInfo() << reply.error();
            }
        }
    });


    /* 修改密码页面 */
    ui->editcheck_curpasswd->setEchoMode(QLineEdit::Password);
    ui->editcheck_newPasswd->setEchoMode(QLineEdit::Password);
    ui->editcheck_confirmPasswd->setEchoMode(QLineEdit::Password);

    //保存按钮
    connect(ui->btn_save, &QPushButton::clicked,
            this, &UserInfoPage::handlerSaveNewPasswd);

    //取消按钮
    connect(ui->btn_cancel, &QPushButton::clicked, [this]() {
        m_errorTip->hideTip();
        ui->stackedWidget->setCurrentIndex(PAGE_USER_INFO);
    });
}

void UserInfoPage::resetPageSetPasswd() {
    ui->editcheck_curpasswd->clear();
    ui->editcheck_newPasswd->clear();
    ui->editcheck_confirmPasswd->clear();
}

void UserInfoPage::handlerSaveNewPasswd() {
    //新密码不能为空
    QString newpasswd = ui->editcheck_newPasswd->text();
    if (newpasswd.isEmpty()) {
        m_errorTip->setText(tr("Please enter the new user password"));
        m_errorTip->showTipAroundWidget(ui->editcheck_newPasswd);
        return;
    }
    //确认新密码不为空，并且和新密码相同
    QString confirmNewPasswd = ui->editcheck_confirmPasswd->text();
    if (confirmNewPasswd.isEmpty()) {
        m_errorTip->setText(tr("Please enter the password again"));
        m_errorTip->showTipAroundWidget(ui->editcheck_confirmPasswd);
        return;
    }
    if (newpasswd != confirmNewPasswd) {
        m_errorTip->setText(tr("The password you enter must be the same as the former one"));
        m_errorTip->showTipAroundWidget(ui->editcheck_confirmPasswd);
        return;
    }
    //当前密码校验
    if (ui->passwd_row_1->isVisible() && ui->passwd_row_2->isVisible()) {
        //当前密码不能为空
        QString curpasswd = ui->editcheck_curpasswd->text();
        if (curpasswd.isEmpty()) {
            m_errorTip->setText(tr("Please enter the current user password"));
            m_errorTip->showTipAroundWidget(ui->editcheck_curpasswd);
            return;
        }
        if (!PasswdHelper::checkUserPassword(ui->label_name->text(), curpasswd)) {
            m_errorTip->setText(tr("The current password is incorrect"));
            m_errorTip->showTipAroundWidget(ui->editcheck_curpasswd);
            return;
        }
        //当前密码是否和新密码相同
        if (curpasswd == newpasswd) {
            m_errorTip->setText(tr("The new password cannot be the same as the current password"));
            m_errorTip->showTipAroundWidget(ui->editcheck_newPasswd);
            return;
        }
    }
    //密码加密
    QString encryptedPasswd;
    if (!PasswdHelper::encryptPassword(newpasswd, encryptedPasswd)) {
        QMessageBox::warning(this, tr("Error"), tr("Password encryption failed"));
        return;
    }
    UserInterface interface(m_curShowUserPath, QDBusConnection::systemBus());
    QDBusPendingReply<> passwdReply = interface.SetPassword(encryptedPasswd, "");
    passwdReply.waitForFinished();
    if (passwdReply.isError()) {
        qWarning() << "SetPassword Failed." << passwdReply.error();
        QMessageBox::warning(this, tr("Error"), tr("Failed to modify password"));
        return;
    }
    m_errorTip->hideTip();
    ui->stackedWidget->setCurrentIndex(PAGE_USER_INFO);
}

void UserInfoPage::handlerSaveUserProperty() {
    UserInterface userInterface(m_curShowUserPath,
                                QDBusConnection::systemBus());

    //icon file
    QString iconFilePath = ui->avatar->iconPath();
    if ( userInterface.icon_file() != iconFilePath ) {
        QDBusPendingReply<> pendingReply = userInterface.SetIconFile(iconFilePath);
        pendingReply.waitForFinished();
        if( pendingReply.isError() ){
            qWarning() << "Save User Property(Icon) Error" << pendingReply.error();
        }
    }

    //account type
    int accountType = ui->combo_accountType->currentIndex();
    if (userInterface.account_type() != accountType) {
        QDBusPendingReply<> pendingReply = userInterface.SetAccountType(accountType);
        pendingReply.waitForFinished();
        if( pendingReply.isError() ){
            qWarning() << "Save User Property(AccountType) Error" << pendingReply.error();
        }
    }

    //locked
    int locked = !ui->checkBox->isChecked();
    if (userInterface.locked() != locked) {
        QDBusPendingReply<> pendingReply = userInterface.SetLocked(locked);
        pendingReply.waitForFinished();
        if( pendingReply.isError() ){
            qWarning() << "Save User Property(Locked) Error" << pendingReply.error();
        }
    }

    ///如果属性设置成功了AccountsGlobalInfo会更新当前页面
    ///手动更新是为了避免设置失败,界面未复位
    updateInfo();
}
