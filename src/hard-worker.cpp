//
// Created by lxh on 2020/11/5.
//

#include "hard-worker.h"
#include "accounts-interface.h"
#include "accounts-user-interface.h"
#include "global-defines.h"
#include "accounts-interface.h"

#include <QDebug>
#include <QDBusConnection>

HardWorker::HardWorker(): QObject(nullptr){

}

HardWorker::~HardWorker() {

}

void HardWorker::doCreateUser(QString account,
                              int uid,
                              int accountType,
                              QString encryptedPasswd,
                              QString homeDir,
                              QString shell,
                              QString iconFile) {
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
        emit sigCreateUserDnoe("", tr("Create User failed"));
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
    qInfo() << QString("create user(%1) is done").arg(account);
    emit sigCreateUserDnoe(userObj, msg);
}

void HardWorker::doUpdatePasswd(QString objPath,
                                QString account,
                                QString encryptedPasswd) {
    UserInterface interface(objPath,QDBusConnection::systemBus());
    QDBusPendingReply<> reply = interface.SetPassword(encryptedPasswd,"");
    reply.waitForFinished();
    if(reply.isError()){
        qWarning() << "set passwd failed," << reply.error();
        emit sigUpdatePasswdDone(tr(" update password failed"));
    }else{
        qInfo() << "update passwd is done";
        emit sigUpdatePasswdDone("");
    }
}

void HardWorker::doUpdateUserProperty(QString objPath,
                                      QString account,
                                      QString iconfile,
                                      int accountType,
                                      bool isLocked) {
    UserInterface userInterface(objPath,
                                QDBusConnection::systemBus());
    QStringList updateFailedPropertys;

    if( userInterface.icon_file() != iconfile ){
        auto reply = userInterface.SetIconFile(iconfile);
        reply.waitForFinished();
        if(reply.isError()){
            qWarning() << "update icon file failed," << reply.error();
            updateFailedPropertys.append(tr("icon file"));
        }
    }

    if( userInterface.account_type() != accountType ){
        auto reply = userInterface.SetAccountType(accountType);
        reply.waitForFinished();
        if(reply.isError()){
            qWarning() << "update account type failed," << reply.error();
            updateFailedPropertys.append(tr("account type"));
        }
    }

    if( userInterface.locked() != isLocked ){
        auto reply = userInterface.SetLocked(isLocked);
        reply.waitForFinished();
        if(reply.isError()){
            qWarning() << "update locked failed," << reply.error();
            updateFailedPropertys.append(tr("locked"));
        }
    }

    /// 更新属性失败
    if( !updateFailedPropertys.isEmpty() ){
        QString updateFailed = updateFailedPropertys.join(",");
        QString msg = QString(tr("Failed to update user properties(%1)"))
                .arg(updateFailed);
        qWarning() << msg;
        emit sigUpdateUserPropertyDone(msg);
    }else{
        qInfo() << "update user property done";
        emit sigUpdateUserPropertyDone("");
    }
}

void HardWorker::doDeleteUser(int uid) {
    AccountsInterface interface(QDBusConnection::systemBus());
    auto reply = interface.DeleteUser(uid,true);
    reply.waitForFinished();
    if(reply.isError()){
        emit sigDeleteUserDone(tr("Failed to delete user"));
    }else{
        emit sigDeleteUserDone("");
    }
}

