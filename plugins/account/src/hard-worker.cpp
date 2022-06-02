/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-account is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "hard-worker.h"
#include "global-defines.h"
#include "ksd_accounts_proxy.h"
#include "ksd_accounts_user_proxy.h"

#include <kiran-system-daemon/accounts-i.h>
#include <qt5-log-i.h>
#include <QDBusConnection>
#include <QDebug>

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
    KSDAccountsProxy accountsService(ACCOUNTS_DBUS_NAME,
                                     ACCOUNTS_OBJECT_PATH,
                                     QDBusConnection::systemBus());
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
        KSDAccountsUserProxy userInterface(ACCOUNTS_DBUS_NAME,
                                           userObjPath,
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
        KSDAccountsUserProxy userInterface(ACCOUNTS_DBUS_NAME,
                                           userObjPath,
                                           QDBusConnection::systemBus());
        qulonglong userID = userInterface.uid();
        KSDAccountsProxy accountsInterface(ACCOUNTS_DBUS_NAME, ACCOUNTS_OBJECT_PATH, QDBusConnection::systemBus());
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
    KSDAccountsUserProxy userProxy(ACCOUNTS_DBUS_NAME, objPath, QDBusConnection::systemBus());
    QDBusPendingReply<> reply = userProxy.SetPassword(encryptedPasswd, "");
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
    KSDAccountsUserProxy userProxy(ACCOUNTS_DBUS_NAME, objPath,
                                   QDBusConnection::systemBus());
    QStringList updateFailedPropertys;

    if (userProxy.icon_file() != iconfile)
    {
        auto reply = userProxy.SetIconFile(iconfile);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "update icon file failed," << reply.error();
            updateFailedPropertys.append(tr("icon file"));
        }
    }

    if (userProxy.account_type() != userType)
    {
        auto reply = userProxy.SetAccountType(userType);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING() << "update userName type failed," << reply.error();
            updateFailedPropertys.append(tr("userName type"));
        }
    }

    if (userProxy.locked() != isLocked)
    {
        auto reply = userProxy.SetLocked(isLocked);
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
    KSDAccountsProxy accountsProxy(ACCOUNTS_DBUS_NAME,
                                   ACCOUNTS_OBJECT_PATH,
                                   QDBusConnection::systemBus());
    // 删除用户
    auto reply = accountsProxy.DeleteUser(uid, true);
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
