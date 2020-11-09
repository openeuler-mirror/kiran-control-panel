#include "create-user-page.h"
#include "ui_create-user-page.h"
#include "advance-settings.h"
#include "passwd-helper.h"
#include "accounts-global-info.h"
#include "kiran-tips.h"

#include <QListView>
#include <QRegExpValidator>
#include <QDebug>
#include <QMessageBox>
#include <kiranwidgets-qt5/kiran-message-box.h>

CreateUserPage::CreateUserPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateUserPage)
{
    ui->setupUi(this);
    initUI();
}

CreateUserPage::~CreateUserPage()
{
    delete ui;
}

void CreateUserPage::reset()
{
    ui->avatar->setDefaultImage();
    ui->edit_name->clear();
    ui->combo_accountType->setCurrentIndex(0);
    ui->editcheck_passwd->resetVerificationStatus();
    ui->editcheck_passwd->clear();
    ui->editcheck_confirmPasswd->resetVerificationStatus();
    ui->editcheck_confirmPasswd->clear();
    m_errorTip->hideTip();

    m_advanceSettingsInfo.clear();
}

void CreateUserPage::setAvatarIconPath(const QString &iconPath)
{
    ui->avatar->setImage(iconPath);
}

//初始化界面
void CreateUserPage::initUI()
{
    /// 提示框
    m_errorTip = new KiranTips(this);
    m_errorTip->setShowPosition(KiranTips::POSITION_BOTTM);
    m_errorTip->setAnimationEnable(true);

    /// 用户头像
    ui->avatar->setHoverImage(":/images/change_user_icon.png");
    ui->avatar->setClickEnable(true);
    connect(ui->avatar,&UserAvatarWidget::pressed,[this](){
        emit sigSetIconForNewUser(ui->avatar->iconPath());
    });

    /// 用户类型ComboBox
    QListView *view =  new QListView(ui->combo_accountType);
    ui->combo_accountType->setView(view);
    ui->combo_accountType->addItem(tr("standard"));
    ui->combo_accountType->addItem(tr("administrator"));
    ui->combo_accountType->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    /// 用户账户名输入框
    QRegExpValidator* validator = new QRegExpValidator(ui->edit_name);
    QRegExp regExp("^\\w+$");
    validator->setRegExp(regExp);
    ui->edit_name->setValidator(validator);
    //NOTE:用户名不能超过32字符长
    ui->edit_name->setMaxLength(32);

    /// 密码输入框
    ui->editcheck_passwd->setMaxLength(24);
    ui->editcheck_passwd->setEchoMode(QLineEdit::Password);
    ui->editcheck_confirmPasswd->setMaxLength(24);
    ui->editcheck_confirmPasswd->setEchoMode(QLineEdit::Password);

    /// 高级设置按钮
    connect(ui->btn_advanceSetting,&QPushButton::clicked,[this](){
        if( ui->edit_name->text().isEmpty() ){
            m_errorTip->setText(tr("Please enter account name first"));
            m_errorTip->showTipAroundWidget(ui->edit_name);
            return;
        }
        AdvanceSettings advanceSettings;
        advanceSettings.show();
        advanceSettings.setInfo(ui->edit_name->text(),m_advanceSettingsInfo);
        QEventLoop eventLoop;
        connect(&advanceSettings,&AdvanceSettings::sigClose,&eventLoop,&QEventLoop::quit);
        eventLoop.exec();
        if(advanceSettings.isConfirm()){
            m_advanceSettingsInfo = advanceSettings.getInfo();
        }
    });

    /// 确认按钮
    connect(ui->btn_confirm,&QPushButton::clicked,this,&CreateUserPage::handlerCreateNewUser);

    /// 取消按钮
    connect(ui->btn_cancel,&QPushButton::clicked,[this](){
        reset();
    });
}

void CreateUserPage::handlerCreateNewUser() {
    //step1.检验账户名是否为空，是否重名
    qInfo() << "start check account name";
    QString account = ui->edit_name->text();

    if( account.isEmpty() ){
        m_errorTip->setText(tr("Please enter your account name"));
        m_errorTip->showTipAroundWidget(ui->edit_name);
        return;
    }
    if( !AccountsGlobalInfo::instance()->checkUserNameAvaliable(account) ){
        m_errorTip->setText(tr("Account already exists"));
        m_errorTip->showTipAroundWidget(ui->edit_name);
        return;
    }

    //step2.检验密码、确认密码是否为空，是否相等
    qInfo() << "start check passwd,confirm passwd";
    QString passwd  = ui->editcheck_passwd->text();
    QString confirmPasswd = ui->editcheck_confirmPasswd->text();
    if( passwd.isEmpty() ){
        ui->editcheck_passwd->setVerificationStatus(false);
        m_errorTip->setText(tr("Please enter your password"));
        m_errorTip->showTipAroundWidget(ui->editcheck_passwd);
        return;
    }
    if( confirmPasswd.isEmpty() ){
        ui->editcheck_confirmPasswd->setVerificationStatus(false);
        m_errorTip->setText(tr("Please enter the password again"));
        m_errorTip->showTipAroundWidget(ui->editcheck_confirmPasswd);
        return;
    }
    if( passwd != confirmPasswd ){
        ui->editcheck_confirmPasswd->setVerificationStatus(false);
        m_errorTip->setText(tr("The password you enter must be the same as the former one"));
        m_errorTip->showTipAroundWidget(ui->editcheck_confirmPasswd);
        return;
    }

    //step3.调用crypt密码加密
    qInfo() << "start encrypted passwd";
    QString encryptedPasswd;
    if( !PasswdHelper::encryptPassword(passwd,encryptedPasswd) ){
        QMessageBox::warning(this,tr("Error"),tr("Password encryption failed"));
        return;
    }

    qint64 uid = -1;
    if( !m_advanceSettingsInfo.uid.isEmpty() ){
        bool toNumberOk = false;
        uid = m_advanceSettingsInfo.uid.toLongLong(&toNumberOk);
        if( !toNumberOk ){
            uid = -1;
        }
    }
    int accountType = ui->combo_accountType->currentIndex();
    QString homeDir = m_advanceSettingsInfo.homeDir;
    QString shell = m_advanceSettingsInfo.shell;
    QString iconFile = ui->avatar->iconPath();

    emit sigIsBusyChanged(true);
    ui->btn_confirm->setBusy(true);
    emit sigCreateUser(account,uid,accountType,
                       encryptedPasswd,
                       homeDir,
                       shell,
                       iconFile);
}

void CreateUserPage::handlerCreateNewUserIsDone(QString userPath,
                                                QString errMsg) {
    emit sigIsBusyChanged(false);
    ui->btn_confirm->setBusy(false);
    if(!errMsg.isEmpty()){
        KiranMessageBox::message(nullptr,tr("Error"),
                                 errMsg,KiranMessageBox::Yes|KiranMessageBox::No);
    }
    if(!userPath.isEmpty()){
        emit sigRequestSetCurrentUser(userPath);
    }
}
