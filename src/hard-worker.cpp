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
 
#include "hard-worker.h"
#include "accounts-interface.h"
#include "accounts-user-interface.h"
#include "global-defines.h"

#include <QDBusConnection>
#include <QDebug>
#include <qt5-log-i.h>

HardWorker::HardWorker() : QObject(nullptr)
{
}

HardWorker::~HardWorker()
{
}

//TODO: 拼接DBus接口返回的错误信息
void HardWorker::doCreateUser(QString userName,
                              int uid,
                              int userType,
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
    createUserRep = accountsService.CreateUser(userName,
                                               userName,
                                               userType,
                                               uid);
    createUserRep.waitForFinished();
    if (createUserRep.isError())
    {
        KLOG_WARNING() << "create user failed," << createUserRep.error();
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
            KLOG_WARNING() << "set passwd failed," << setpwdRep.error();
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
                KLOG_WARNING() << "set home directory failed," << setHomeRep.error();
                errMsgDetail = setHomeRep.error().message();
                goto failed;
            }
        }
        ///step4. 设置shell
        QDBusPendingReply<> setShellRep = userInterface.SetShell(shell.isEmpty() ? DEFAULT_SHELL : shell);
        setShellRep.waitForFinished();
        if (setShellRep.isError())
        {
            KLOG_WARNING() << "set shell failed," << setShellRep.error();
            errMsgDetail = setShellRep.error().message();
            goto failed;
        }
        ///step5. 设置图标
        QDBusPendingReply<> setIconRep = userInterface.SetIconFile(iconFile);
        setIconRep.waitForFinished();
        if (setIconRep.isError())
        {
            KLOG_WARNING() << "set icon failed," << setIconRep.error();
            errMsgDetail = setIconRep.error().message();
            goto failed;
        }
    }

    KLOG_INFO() << QString("create user(%1) is done").arg(userName);
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
            KLOG_WARNING() << "create user failed,delete user:" << reply.error();
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
                                QString userName,
                                QString encryptedPasswd)
{
    UserInterface interface(objPath, QDBusConnection::systemBus());
    QDBusPendingReply<> reply = interface.SetPassword(encryptedPasswd, "");
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "set passwd failed," << reply.error();
        emit sigUpdatePasswdDone(tr(" update password failed"));
    }
    else
    {
        KLOG_INFO() << "update passwd is done";
        emit sigUpdatePasswdDone("");
    }
}

void HardWorker::doUpdateUserProperty(QString objPath,
                                      QString userName,
                                      QString iconfile,
                                      int userType,
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
            KLOG_WARNING() << "update icon file failed," << reply.error();
            updateFailedPropertys.append(tr("icon file"));
        }
    }

    if (userInterface.account_type() != userType)
    {
        auto reply = userInterface.SetAccountType(userType);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "update userName type failed," << reply.error();
            updateFailedPropertys.append(tr("userName type"));
        }
    }

    if (userInterface.locked() != isLocked)
    {
        auto reply = userInterface.SetLocked(isLocked);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "update locked failed," << reply.error();
            updateFailedPropertys.append(tr("locked"));
        }
    }

    /// 更新属性失败
    if (!updateFailedPropertys.isEmpty())
    {
        QString updateFailed = updateFailedPropertys.join(",");
        QString msg = QString(tr("Failed to update user properties,%1"))
                          .arg(updateFailed);
        KLOG_WARNING() << msg;
        emit sigUpdateUserPropertyDone(msg);
    }
    else
    {
        KLOG_INFO() << "update user property done";
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
        KLOG_INFO() << "delete user" << reply.error();
        QString errMsg = QString(tr("Failed to delete user,%1")).arg(reply.error().message());
        emit sigDeleteUserDone(errMsg);
    }
    else
    {
        emit sigDeleteUserDone("");
    }
}
