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
     * @brief 初始化，加载账户列表
     * @return
     */
    bool init();

    /**
     * @brief  获取排序之后的用户列表
     * @return QList<QString> 用户DBusObjectPath列表
     */
    QList<QString> getUserList();

    /**
     * @brief 检查是否存在重名账户
     * @param account 需检查的账户名
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
