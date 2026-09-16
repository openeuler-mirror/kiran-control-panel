/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "hard-worker.h"
#include "kiran-account-service-wrapper.h"
#include "config.h"

#include <kiran-system-daemon/accounts-i.h>
#include <qt5-log-i.h>
#include <QDBusConnection>
#include <QDebug>
#include <QFile>

#include <cerrno>
#include <sys/stat.h>

namespace
{
/// 目录存在状态
enum class DirExistence
{
    EXISTS,      ///< 已存在（含被同名文件占用）
    NOT_EXISTS,  ///< 确认不存在
    UNKNOWN,     ///< 无法判定（如无权限搜索父目录），应保守按 EXISTS 处理
};

/// 探测目录存在状态。不用 QFileInfo::exists()：父目录无搜索权限（EACCES）时
/// 它同样返回 false，会把"探测不到"误判为"不存在"。
static DirExistence probeDirExistence(const QString &path)
{
    struct stat st;
    if (::stat(QFile::encodeName(path).constData(), &st) == 0)
    {
        return DirExistence::EXISTS;
    }

    if (errno == ENOENT || errno == ENOTDIR)  // 查无此项 / 路径中某组件不是目录
    {
        return DirExistence::NOT_EXISTS;
    }

    return DirExistence::UNKNOWN;  // EACCES 等：无法确认目录是否真实存在
}

/// 目录存在状态的文本描述（用于日志）
static const char *dirExistenceName(DirExistence existence)
{
    switch (existence)
    {
    case DirExistence::EXISTS:
        return "exists";
    case DirExistence::NOT_EXISTS:
        return "not exists";
    default:
        return "unknown";
    }
}
}  // namespace

HardWorker::HardWorker() : QObject(nullptr)
{
}

HardWorker::~HardWorker()
{
}

void HardWorker::doCreateUser(QString userName,
                              int uid,
                              int userType,
                              QString encryptedPasswd,
                              QString homeDir,
                              QString shell,
                              QString iconFile)
{
    auto accountsServiceAPI = DBusWrapper::createKiranAccountServiceAPI();
    QString userObjPath;
    QString errMsgDetail;

    auto createUserDoneWithError = [this](const QString& errorDetail) -> void
    {
        QString errMsgPrefix = tr("Create User failed");
        QString errMsg = errMsgPrefix;
        if (!errorDetail.isEmpty())
        {
            errMsg.append(",");
            errMsg.append(errorDetail);
        }
        emit sigCreateUserDnoe("", errMsg);
    };

    if (accountsServiceAPI.isNull())
    {
        KLOG_WARNING(qLcAccount) << "create user failed,can not get kiran account service backend!";
        return createUserDoneWithError(tr("Failed to connect to the account management service"));
    }

    /// step1.创建用户
    QDBusPendingReply<QDBusObjectPath> createUserRep;
    createUserRep = accountsServiceAPI->CreateUser(userName,
                                                   userName,
                                                   userType,
                                                   uid);
    createUserRep.waitForFinished();
    if (createUserRep.isError())
    {
        KLOG_WARNING(qLcAccount) << "create user failed," << createUserRep.error();
        errMsgDetail = createUserRep.error().message();
        return createUserDoneWithError(errMsgDetail);
    }

    userObjPath = createUserRep.value().path();
    auto userInterface = DBusWrapper::createKiranAccountServiceUserAPI(userObjPath);

    /// 回退删除用户时是否连带清理用户主目录。仅当主目录属本次流程产物时为 true：
    /// 未指定目录（useradd 生成的默认目录），或指定目录确认不存在（由本次流程新建）。
    /// 指定目录已存在或无法判定时必须为 false，防止回退误删用户既有数据。
    bool removeHomeDirOnRollback = true;
    auto deleteUserAndReplyError = [this, createUserDoneWithError,
                                    accountsServiceAPI, userInterface,
                                    &removeHomeDirOnRollback](const QString& errorDetail) -> void
    {
        auto uid = userInterface->uid();

        KLOG_INFO(qLcAccount) << "rollback delete user, uid:" << uid
                              << ", remove home directory:" << removeHomeDirOnRollback
                              << ", reason:" << errorDetail;

        auto reply = accountsServiceAPI->DeleteUser(uid, removeHomeDirOnRollback);
        reply.waitForFinished();

        createUserDoneWithError(errorDetail);
    };

    /// step2. 设置密码
    QDBusPendingReply<> setpwdRep = userInterface->SetPasswordByPasswd("", encryptedPasswd);
    setpwdRep.waitForFinished();
    if (setpwdRep.isError())
    {
        KLOG_WARNING(qLcAccount) << "set passwd failed," << setpwdRep.error();
        errMsgDetail = setpwdRep.error().message();
        return deleteUserAndReplyError(errMsgDetail);
    }

    /// step3.　设置Home
    if (!homeDir.isEmpty())
    {
        /// 指定目录仅当确认不存在时才由本次流程新建，回退可随之清理；否则视为
        /// 用户既有数据目录，回退不得清理——SetHomeDirectory 失败时 passwd 中的
        /// home 也可能已被 usermod 改写为该目录（改写不随 usermod 失败回退），
        /// 清理会误删其中数据。
        DirExistence homeDirExistence = probeDirExistence(homeDir);
        removeHomeDirOnRollback = (homeDirExistence == DirExistence::NOT_EXISTS);
        KLOG_INFO(qLcAccount) << "probe specified home directory:" << homeDir
                              << ", existence:" << dirExistenceName(homeDirExistence)
                              << ", remove home directory on rollback:" << removeHomeDirOnRollback;

        QDBusPendingReply<> setHomeRep = userInterface->SetHomeDirectory(homeDir);
        setHomeRep.waitForFinished();
        if (setHomeRep.isError())
        {
            KLOG_WARNING(qLcAccount) << "set home directory failed," << setHomeRep.error();
            errMsgDetail = setHomeRep.error().message();
            return deleteUserAndReplyError(errMsgDetail);
        }
    }

    /// step4. 设置shell
    QDBusPendingReply<> setShellRep = userInterface->SetShell(shell.isEmpty() ? ACCOUNT_DEFAULT_SHELL : shell);
    setShellRep.waitForFinished();
    if (setShellRep.isError())
    {
        KLOG_WARNING(qLcAccount) << "set shell failed," << setShellRep.error();
        errMsgDetail = setShellRep.error().message();
        return deleteUserAndReplyError(errMsgDetail);
    }

    /// step5. 设置图标
    QDBusPendingReply<> setIconRep = userInterface->SetIconFile(iconFile);
    setIconRep.waitForFinished();
    if (setIconRep.isError())
    {
        KLOG_WARNING(qLcAccount) << "set icon failed," << setIconRep.error();
        errMsgDetail = setIconRep.error().message();
        return deleteUserAndReplyError(errMsgDetail);
    }

    KLOG_INFO(qLcAccount,"create user(%s) is done",userName.toStdString().c_str());
    emit sigCreateUserDnoe(userObjPath, "");
    return;
}

void HardWorker::doUpdatePasswd(QString objPath,
                                QString userName,
                                QString encryptedCurPasswd,
                                QString encryptedPasswd)
{
    auto userProxy = DBusWrapper::createKiranAccountServiceUserAPI(objPath);

    QDBusPendingReply<> reply = userProxy->SetPasswordByPasswd(encryptedCurPasswd, encryptedPasswd);
    reply.waitForFinished();

    if (reply.isError())
    {
        KLOG_ERROR(qLcAccount) << "set passwd failed," << reply.error();
        QString errMsg = QString("%1,%2").arg(tr(" update password failed")).arg(reply.error().message());
        emit sigUpdatePasswdDone(errMsg);
    }
    else
    {
        KLOG_INFO(qLcAccount) << "update passwd is done";
        emit sigUpdatePasswdDone("");
    }
}

void HardWorker::doUpdateUserProperty(QString objPath,
                                      QString userName,
                                      QString iconfile,
                                      int userType,
                                      bool isLocked)
{
    auto userProxy = DBusWrapper::createKiranAccountServiceUserAPI(objPath);
    QStringList updateFailedPropertys;

    if (userProxy->icon_file() != iconfile)
    {
        auto reply = userProxy->SetIconFile(iconfile);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING(qLcAccount) << "update icon file failed," << reply.error();
            updateFailedPropertys.append(tr("icon file"));
        }
    }

    if (userProxy->account_type() != userType)
    {
        auto reply = userProxy->SetAccountType(userType);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING(qLcAccount) << "update userName type failed," << reply.error();
            updateFailedPropertys.append(tr("userName type"));
        }
    }

    if (userProxy->locked() != isLocked)
    {
        auto reply = userProxy->SetLocked(isLocked);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_WARNING(qLcAccount) << "update locked failed," << reply.error();
            updateFailedPropertys.append(tr("locked"));
        }
    }

    /// 更新属性失败
    if (!updateFailedPropertys.isEmpty())
    {
        QString updateFailed = updateFailedPropertys.join(",");
        KLOG_WARNING(qLcAccount,"failed to update user properties: %s",updateFailed.toStdString().c_str());

        QString msg = QString(tr("Failed to update user properties,%1"))
                          .arg(updateFailed);
        emit sigUpdateUserPropertyDone(msg);
    }
    else
    {
        KLOG_INFO(qLcAccount) << "update user property done";
        emit sigUpdateUserPropertyDone("");
    }
}

void HardWorker::doDeleteUser(int uid)
{
    auto accountsProxy = DBusWrapper::createKiranAccountServiceAPI();

    auto reply = accountsProxy->DeleteUser(uid, true);
    reply.waitForFinished();

    if (reply.isError())
    {
        KLOG_INFO(qLcAccount) << "delete user error:" << reply.error();

        QString errMsg = QString(tr("Failed to delete user,%1")).arg(reply.error().message());
        emit sigDeleteUserDone(errMsg);
        return;
    }

    emit sigDeleteUserDone("");
}
