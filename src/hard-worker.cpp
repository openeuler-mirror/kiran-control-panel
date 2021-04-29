//
// Created by lxh on 2020/11/5.
//

#include "hard-worker.h"
#include "accounts-interface.h"
#include "accounts-user-interface.h"
#include "global-defines.h"
#include "log.h"

#include <QDBusConnection>
#include <QDebug>

HardWorker::HardWorker() : QObject(nullptr)
{
}

HardWorker::~HardWorker()
{
}

//TODO: 拼接DBus接口返回的错误信息
void HardWorker::doCreateUser(QString account,
                              int uid,
                              int accountType,
                              QString encryptedPasswd,
                              QString homeDir,
                              QString shell,
                              QString iconFile)
{
    AccountsInterface accountsService(QDBusConnection::systemBus());
    QString userObjPath;
    QString errMsgPrefix = tr("Create User failed");
    QString errMsgDetail;

    ///step1.创建用户
    QDBusPendingReply<QDBusObjectPath> createUserRep;
    createUserRep = accountsService.CreateUser(account,
                                               account,
                                               accountType,
                                               uid);
    createUserRep.waitForFinished();
    if (createUserRep.isError())
    {
        LOG_WARNING_S() << "create user failed," << createUserRep.error();
        errMsgDetail = createUserRep.error().message();
        goto failed;
    }

    userObjPath = createUserRep.value().path();

    {
        UserInterface userInterface(userObjPath,
                                    QDBusConnection::systemBus());
        ///step2. 设置密码
        QDBusPendingReply<> setpwdRep = userInterface.SetPassword(encryptedPasswd, "");
        setpwdRep.waitForFinished();
        if (setpwdRep.isError())
        {
            LOG_WARNING_S() << "set passwd failed," << setpwdRep.error();
            errMsgDetail = setpwdRep.error().message();
            goto failed;
        }
        ///step3.　设置Home
        if (!homeDir.isEmpty())
        {
            QDBusPendingReply<> setHomeRep = userInterface.SetHomeDirectory(homeDir);
            setHomeRep.waitForFinished();
            if (setHomeRep.isError())
            {
                LOG_WARNING_S() << "set home directory failed," << setHomeRep.error();
                errMsgDetail = setHomeRep.error().message();
                goto failed;
            }
        }
        ///step4. 设置shell
        QDBusPendingReply<> setShellRep = userInterface.SetShell(shell.isEmpty() ? DEFAULT_SHELL : shell);
        setShellRep.waitForFinished();
        if (setShellRep.isError())
        {
            LOG_WARNING_S() << "set shell failed," << setShellRep.error();
            errMsgDetail = setShellRep.error().message();
            goto failed;
        }
        ///step5. 设置图标
        QDBusPendingReply<> setIconRep = userInterface.SetIconFile(iconFile);
        setIconRep.waitForFinished();
        if (setIconRep.isError())
        {
            LOG_WARNING_S() << "set icon failed," << setIconRep.error();
            errMsgDetail = setIconRep.error().message();
            goto failed;
        }
    }

    LOG_INFO_S() << QString("create user(%1) is done").arg(account);
    emit sigCreateUserDnoe(userObjPath, "");
    return;
failed:
    if (!userObjPath.isEmpty())
    {
        UserInterface userInterface(userObjPath,
                                    QDBusConnection::systemBus());
        qulonglong userID = userInterface.uid();
        AccountsInterface accountsInterface(QDBusConnection::systemBus());
        auto reply = accountsInterface.DeleteUser(userID, true);
        reply.waitForFinished();
        if (reply.isError())
        {
            LOG_WARNING_S() << "create user failed,delete user:" << reply.error();
        }
    }
    QString errMsg = errMsgPrefix;
    if (!errMsgDetail.isEmpty())
    {
        errMsg.append(",");
        errMsg.append(errMsgDetail);
    }
    emit sigCreateUserDnoe("", errMsg);
}

void HardWorker::doUpdatePasswd(QString objPath,
                                QString account,
                                QString encryptedPasswd)
{
    UserInterface interface(objPath, QDBusConnection::systemBus());
    QDBusPendingReply<> reply = interface.SetPassword(encryptedPasswd, "");
    reply.waitForFinished();
    if (reply.isError())
    {
        LOG_WARNING_S() << "set passwd failed," << reply.error();
        emit sigUpdatePasswdDone(tr(" update password failed"));
    }
    else
    {
        LOG_INFO_S() << "update passwd is done";
        emit sigUpdatePasswdDone("");
    }
}

void HardWorker::doUpdateUserProperty(QString objPath,
                                      QString account,
                                      QString iconfile,
                                      int accountType,
                                      bool isLocked)
{
    UserInterface userInterface(objPath,
                                QDBusConnection::systemBus());
    QStringList updateFailedPropertys;

    if (userInterface.icon_file() != iconfile)
    {
        auto reply = userInterface.SetIconFile(iconfile);
        reply.waitForFinished();
        if (reply.isError())
        {
            LOG_WARNING_S() << "update icon file failed," << reply.error();
            updateFailedPropertys.append(tr("icon file"));
        }
    }

    if (userInterface.account_type() != accountType)
    {
        auto reply = userInterface.SetAccountType(accountType);
        reply.waitForFinished();
        if (reply.isError())
        {
            LOG_WARNING_S() << "update account type failed," << reply.error();
            updateFailedPropertys.append(tr("account type"));
        }
    }

    if (userInterface.locked() != isLocked)
    {
        auto reply = userInterface.SetLocked(isLocked);
        reply.waitForFinished();
        if (reply.isError())
        {
            LOG_WARNING_S() << "update locked failed," << reply.error();
            updateFailedPropertys.append(tr("locked"));
        }
    }

    /// 更新属性失败
    if (!updateFailedPropertys.isEmpty())
    {
        QString updateFailed = updateFailedPropertys.join(",");
        QString msg = QString(tr("Failed to update user properties(%1)"))
                          .arg(updateFailed);
        LOG_WARNING_S() << msg;
        emit sigUpdateUserPropertyDone(msg);
    }
    else
    {
        LOG_INFO_S() << "update user property done";
        emit sigUpdateUserPropertyDone("");
    }
}

void HardWorker::doDeleteUser(int uid)
{
    AccountsInterface interface(QDBusConnection::systemBus());
    auto reply = interface.DeleteUser(uid, true);
    reply.waitForFinished();
    if (reply.isError())
    {
        LOG_INFO_S() << "delete user" << reply.error();
        emit sigDeleteUserDone(tr("Failed to delete user"));
    }
    else
    {
        emit sigDeleteUserDone("");
    }
}
