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

#include "accounts-global-info.h"
#include "def.h"
#include "ksd_accounts_proxy.h"
#include "ksd_accounts_user_proxy.h"

#include <kiran-system-daemon/accounts-i.h>
#include <qt5-log-i.h>
#include <unistd.h>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#include <QList>
#include <QMutex>
#include <QScopedPointer>

AccountsGlobalInfo::AccountsGlobalInfo(QObject *parent)
    : QObject(parent),
      m_accountsInterface(ACCOUNTS_DBUS_NAME, ACCOUNTS_OBJECT_PATH, QDBusConnection::systemBus())
{
}

AccountsGlobalInfo::~AccountsGlobalInfo()
{
}

AccountsGlobalInfo *AccountsGlobalInfo::instance()
{
    static QMutex mutex;
    static QScopedPointer<AccountsGlobalInfo> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new AccountsGlobalInfo);
        }
    }

    return pInst.data();
}

bool AccountsGlobalInfo::init()
{
    connect(&m_accountsInterface, &KSDAccountsProxy::UserAdded, [this](const QDBusObjectPath &user)
            { addUserToMap(user); });
    connect(&m_accountsInterface, &KSDAccountsProxy::UserDeleted, [this](const QDBusObjectPath &user)
            { deleteUserFromMap(user); });

    /// 加载用户
    QList<QDBusObjectPath> users;
    QDBusPendingReply<QList<QDBusObjectPath>> pendingReply;
    QList<QDBusObjectPath> objList;
    QList<QDBusObjectPath>::iterator objListIter;
    pendingReply = m_accountsInterface.GetNonSystemUsers();
    pendingReply.waitForFinished();
    if (pendingReply.isError())
    {
        KLOG_ERROR() << "GetNonSystemUsers Error:"
                     << pendingReply.error();
        return false;
    }
    objList = pendingReply.value();

    for (objListIter = objList.begin();
         objListIter != objList.end();
         ++objListIter)
    {
        addUserToMap(*objListIter);
    }
    return true;
}

QList<QString> AccountsGlobalInfo::getUserList()
{
    QList<QString> userObjPathList;
    for (auto iter = m_usersMap.begin();
         iter != m_usersMap.end();
         iter++)
    {
        userObjPathList.append((*iter)->path());
    }
    return userObjPathList;
}

void AccountsGlobalInfo::addUserToMap(const QDBusObjectPath &user)
{
    if (m_usersMap.find(user.path()) != m_usersMap.end())
    {
        return;
    }

    auto userProxy = new KSDAccountsUserProxy(ACCOUNTS_DBUS_NAME,
                                              user.path(),
                                              QDBusConnection::systemBus(),
                                              this);

    connect(userProxy,
            &KSDAccountsUserProxy::dbusPropertyChanged,
            this,
            &AccountsGlobalInfo::handlerPropertyChanged);

    m_usersMap.insert(user.path(), userProxy);
    emit UserAdded(user);
}

void AccountsGlobalInfo::deleteUserFromMap(const QDBusObjectPath &user)
{
    if (m_usersMap.find(user.path()) == m_usersMap.end())
    {
        return;
    }

    auto userProxy = m_usersMap.take(user.path());
    disconnect(userProxy,
               &KSDAccountsUserProxy::dbusPropertyChanged,
               this,
               &AccountsGlobalInfo::handlerPropertyChanged);
    delete userProxy;

    emit UserDeleted(user);
}

void AccountsGlobalInfo::handlerPropertyChanged(const QString &propertyName, const QVariant &value)
{
    auto userProxy = qobject_cast<KSDAccountsUserProxy *>(sender());

    KLOG_DEBUG() << "property changed:" << userProxy->path();
    KLOG_DEBUG() << "\tname: " << propertyName;
    KLOG_DEBUG() << "\tvalue:" << value;

    emit UserPropertyChanged(userProxy->path(), propertyName, value);
}
