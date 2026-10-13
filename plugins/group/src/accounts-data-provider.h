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

#ifndef ACCOUNTSDATAPROVIDER_H
#define ACCOUNTSDATAPROVIDER_H

#include <QDBusObjectPath>
#include <QObject>
#include <QStringList>
#include <QVariantMap>

#define ACCOUNTS_DBUS_NAME "com.kylinsec.Kiran.SystemDaemon.Accounts"
#define ACCOUNTS_OBJECT_PATH "/com/kylinsec/Kiran/SystemDaemon/Accounts"
#define ACCOUNTS_DBUS_INTERFACE_NAME "com.kylinsec.Kiran.SystemDaemon.Accounts"

class QThread;
class KSDAccountsProxy;
class UserLoader;

/// @brief 用户信息的持有者，生命周期跟着使用它的页面控件走
///        不为每个用户创建 D-Bus 代理：用户名在后台线程加载，随 UserAdded 一起下发
class AccountsDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit AccountsDataProvider(QObject *parent = nullptr);
    ~AccountsDataProvider();

    /**
     * @brief 枚举用户并在后台线程加载用户名，页面创建时调用一次
     */
    void loadUsers();

signals:
    void UserAdded(const QDBusObjectPath &obj, const QString &userName);
    void UserDeleted(const QDBusObjectPath &obj);

private Q_SLOTS:
    // 一个用户加载完成
    void handleUserLoaded(const QString &objectPath, const QVariantMap &values);

private:
    void handleUserAdded(const QDBusObjectPath &user);
    void handleUserDeleted(const QDBusObjectPath &user);
    void startBackgroundLoad(const QStringList &objectPaths);
    void stopBackgroundLoad();

private:
    KSDAccountsProxy *m_accountsInterface = nullptr;
    UserLoader *m_loader = nullptr;
    QThread *m_loaderThread = nullptr;
};

#endif  // ACCOUNTSDATAPROVIDER_H
