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

 
#ifndef ACCOUNTSGLOBALINFO_H
#define ACCOUNTSGLOBALINFO_H

#include "ksd_accounts_proxy.h"
#include "ksd_accounts_user_proxy.h"

#include <QList>
#include <QObject>

class AccountsGlobalInfo : public QObject
{
    Q_OBJECT
private:
    explicit AccountsGlobalInfo(QObject *parent = nullptr);

public:
    ~AccountsGlobalInfo();

    static AccountsGlobalInfo *instance();

    /**
     * @brief 初始化，加载用户列表
     * @return
     */
    bool init();

    /**
     * @brief  获取排序之后的用户列表
     * @return QList<QString> 用户DBusObjectPath列表
     */
    QList<QString> getUserList();

    /**
     * @brief 检查是否存在重名用户
     * @param account 需检查的用户名
     * @return 是否可用
     */
    bool checkUserNameAvaliable(const QString &userName);

    QString getCurrentUser();

private:
    void addUserToMap(const QDBusObjectPath &user);
    void deleteUserFromMap(const QDBusObjectPath &user);

signals:
    void UserAdded(const QDBusObjectPath &obj);
    void UserDeleted(const QDBusObjectPath &obj);
    void UserPropertyChanged(QString userPath,
                             QString propertyName,
                             QVariant value);

private Q_SLOTS:
    void handlerPropertyChanged(const QString &propertyName, const QVariant &value);

private:
    KSDAccountsProxy m_accountsInterface;
    QMap<QString,KSDAccountsUserProxy*> m_usersMap; // QMap<DBus对象路径,用户相关接口>
    QString m_curUserName;
    bool m_showRoot =  false;
};

#endif  // ACCOUNTSGLOBALINFO_H
