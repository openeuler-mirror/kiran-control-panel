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
#ifndef ACCOUNTSDATAPROVIDER_H
#define ACCOUNTSDATAPROVIDER_H

#include "kiran-account-service-wrapper.h"

#include <QList>
#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

class QThread;
class UserLoader;

/// @brief 用户信息的持有者，生命周期跟着使用它的页面控件走
///        不为每个用户创建 D-Bus 代理：展示值在后台线程加载，随 UserAdded 一起下发；
///        属性变化订阅整个服务一次，在槽里按消息里的用户对象路径区分
class AccountsDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit AccountsDataProvider(QObject *parent = nullptr);
    ~AccountsDataProvider();

    QString rsaPublicKey();

    // 检查用户名是否已被占用，未占用返回true；
    // 直接问后端，不依赖本地是否已经把所有用户加载完
    bool checkUserNameAvaliable(const QString &userName);

    QString getCurrentUser();

    // 枚举用户并在后台线程加载展示值，页面创建时调用一次
    void loadUsers();

signals:
    void UserAdded(const QString &objectPath,
                   const QString &userName,
                   const QString &iconFile,
                   bool locked);
    void UserDeleted(const QString& obj);
    void UserPropertyChanged(QString userPath,
                             QString propertyName,
                             QVariant value);

private Q_SLOTS:
    // 服务级订阅的回调，所有用户对象的属性变化都从这里出来
    void handleDbusPropertiesChanged(const QDBusMessage &message);
    // 一个用户加载完成
    void handleUserLoaded(const QString &objectPath, const QVariantMap &values);

private:
    void handleUserAdded(const QDBusObjectPath &user);
    void handleUserDeleted(const QDBusObjectPath &user);
    void emitUserAdded(const QString &objectPath, const QVariantMap &values);
    QString prepareCurrentUser(const QList<QDBusObjectPath> &userList);
    void startBackgroundLoad(const QStringList &objectPaths);
    void stopBackgroundLoad();

private:
    DBusWrapper::KiranAccountServicePtr m_accountsInterface;
    UserLoader *m_loader = nullptr;
    QThread *m_loaderThread = nullptr;
    QString m_curUserName;
    bool m_showRoot =  false;
    QString m_pubkey;
};

#endif  // ACCOUNTSDATAPROVIDER_H
