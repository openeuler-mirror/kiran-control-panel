//
// Created by lxh on 2020/11/5.
//

#include "hard-worker.h"
#include "accounts-interface.h"
#include "accounts-user-interface.h"
#include "global-defines.h"

#include <QDebug>
#include <QDBusConnection>

HardWorker::HardWorker(): QObject(nullptr){

}

HardWorker::~HardWorker() {

}

void HardWorker::doCreateUser(const QString &account, int uid,
                              int accountType, const QString &encryptedPasswd,
                              const QString &homeDir, const QString &shell,
                              const QString &iconFile) {
    AccountsInterface accountsService(QDBusConnection::systemBus());

    ///step1.创建用户
    QDBusPendingReply<QDBusObjectPath> createUserRep;
    createUserRep = accountsService.CreateUser(account,
                                               account,
                                               accountType,
                                               uid);
    createUserRep.waitForFinished();
    if( createUserRep.isError() ){
        qWarning() << "create user failed," << createUserRep.error();
        emit createUserisDnoe("",tr("Create User failed"));
        return;
    }

    QString userObj = createUserRep.value().path();

    UserInterface userInterface(userObj,
                                QDBusConnection::systemBus());
    QList<QString> setPropertyFailedList;
    ///step2. 设置密码
    QDBusPendingReply<> setpwdRep = userInterface.SetPassword(encryptedPasswd,"");
    setpwdRep.waitForFinished();
    if( setpwdRep.isError() ){
        qWarning() << "set passwd failed," << setpwdRep.error();
        setPropertyFailedList.append(tr("password"));
    }

    ///step3.　设置Home
    if( !homeDir.isEmpty() ){
        QDBusPendingReply<> setHomeRep = userInterface.SetHomeDirectory(homeDir);
        setHomeRep.waitForFinished();
        if(setHomeRep.isError()){
            qWarning() << "set home directory failed," << setHomeRep.error();
            setPropertyFailedList.append(tr("home directory"));
        }
    }

    ///step4. 设置shell
    QDBusPendingReply<> setShellRep = userInterface.SetShell(shell.isEmpty()?DEFAULT_SHELL:shell);
    setShellRep.waitForFinished();
    if( setShellRep.isError() ){
        qWarning() << "set shell failed," << setShellRep.error();
        setPropertyFailedList.append(tr("shell"));
    }

    ///step5. 设置图标
    QDBusPendingReply<> setIconRep = userInterface.SetIconFile(iconFile);
    setIconRep.waitForFinished();
    if(setIconRep.isError()){
        qWarning() << "set icon failed," << setIconRep.error();
        setPropertyFailedList.append(tr("icon"));
    }

    QString msg;
    if( setPropertyFailedList.size() > 0 ){
        QString msgPrefix = tr("Failed to set user attributes");
        msg = msgPrefix + "(";
        for( auto iter = setPropertyFailedList.begin();
             iter != setPropertyFailedList.end();
             iter++ ){
            if( iter!=setPropertyFailedList.begin() ){
                msg.append(",");
            }
            msg.append(*iter);
        }
        msg.append(")");
    }

    emit createUserisDnoe(userObj,msg);
}

