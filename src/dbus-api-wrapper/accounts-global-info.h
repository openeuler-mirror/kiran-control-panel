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
 
#ifndef ACCOUNTSGLOBALINFO_H
#define ACCOUNTSGLOBALINFO_H

#include "accounts-interface.h"
#include "accounts-user-interface.h"

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
    void handlerPropertyChanged(QString userPath, QString propertyName, QVariant value);

private:
    AccountsInterface m_accountsInterface;
    QMap<QString,UserInterface *> m_usersMap; // QMap<DBus对象路径,用户相关接口>
    QString m_curUserName;
    bool m_showRoot =  false;
};

#endif  // ACCOUNTSGLOBALINFO_H
