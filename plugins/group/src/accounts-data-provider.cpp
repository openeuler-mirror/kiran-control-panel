/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-group is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     wangshichang <shichang@isrc.iscas.ac.cn>
 */

#include "accounts-data-provider.h"
#include "ksd_accounts_proxy.h"
#include "user-loader.h"

#include <kiran-system-daemon/accounts-i.h>
#include <qt5-log-i.h>
#include <QDBusPendingCall>
#include <QThread>

AccountsDataProvider::AccountsDataProvider(QObject *parent)
    : QObject(parent),
      m_accountsInterface(new KSDAccountsProxy(ACCOUNTS_DBUS_NAME, ACCOUNTS_OBJECT_PATH, QDBusConnection::systemBus()))
{
    connect(m_accountsInterface, &KSDAccountsProxy::UserAdded,
            this, &AccountsDataProvider::handleUserAdded);
    connect(m_accountsInterface, &KSDAccountsProxy::UserDeleted,
            this, &AccountsDataProvider::handleUserDeleted);
}

AccountsDataProvider::~AccountsDataProvider()
{
    stopBackgroundLoad();

    delete m_accountsInterface;
    m_accountsInterface = nullptr;
}

void AccountsDataProvider::loadUsers()
{
    /// 加载用户
    QDBusPendingReply<QList<QDBusObjectPath>> pendingReply = m_accountsInterface->GetNonSystemUsers();
    pendingReply.waitForFinished();
    if (pendingReply.isError())
    {
        KLOG_ERROR() << "GetNonSystemUsers Error:"
                     << pendingReply.error();
        return;
    }

    QStringList allPaths;
    const QList<QDBusObjectPath> objList = pendingReply.value();
    for (auto objListIter = objList.begin();
         objListIter != objList.end();
         ++objListIter)
    {
        allPaths.append(objListIter->path());
    }

    if (!allPaths.isEmpty())
    {
        KLOG_DEBUG() << "load" << allPaths.size() << "users in background";
        startBackgroundLoad(allPaths);
    }
}

void AccountsDataProvider::handleUserAdded(const QDBusObjectPath &user)
{
    // 新增用户的用户名也放到加载器线程加载，不在主线程等D-Bus返回
    startBackgroundLoad(QStringList() << user.path());
}

void AccountsDataProvider::handleUserDeleted(const QDBusObjectPath &user)
{
    KLOG_DEBUG() << "delete user:" << user.path();
    emit UserDeleted(user);
}

void AccountsDataProvider::handleUserLoaded(const QString &objectPath, const QVariantMap &values)
{
    emit UserAdded(QDBusObjectPath(objectPath),
                   values.value(QStringLiteral("user_name")).toString());
}

void AccountsDataProvider::startBackgroundLoad(const QStringList &objectPaths)
{
    if (objectPaths.isEmpty())
    {
        return;
    }

    if (!m_loaderThread)
    {
        m_loaderThread = new QThread(this);
        m_loader = new UserLoader;
        m_loader->moveToThread(m_loaderThread);

        // 线程结束时由 loader 自身线程删除，避免主线程跨线程 delete
        connect(m_loaderThread, &QThread::finished, m_loader, &QObject::deleteLater);
        connect(m_loader, &UserLoader::userLoaded,
                this, &AccountsDataProvider::handleUserLoaded);

        m_loaderThread->start();
    }

    // 队列连接，实际加载在加载器线程里做，加载完一个回调一个
    QMetaObject::invokeMethod(m_loader, "load", Qt::QueuedConnection, Q_ARG(QStringList, objectPaths));
}

void AccountsDataProvider::stopBackgroundLoad()
{
    if (!m_loaderThread)
    {
        return;
    }

    m_loader->requestStop();
    m_loaderThread->quit();

    if (!m_loaderThread->wait(10000))
    {
        KLOG_WARNING() << "user loader did not stop in 10s, detach it";
        // 超时：断开业务连接并放弃管理，避免崩溃
        disconnect(m_loader, nullptr, this, nullptr);
        m_loaderThread->setParent(nullptr);
        m_loader = nullptr;
        m_loaderThread = nullptr;
        return;
    }

    // m_loader 已随线程结束被删除，这里只删线程对象
    m_loader = nullptr;
    delete m_loaderThread;
    m_loaderThread = nullptr;
}
