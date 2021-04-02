#include "accounts-global-info.h"
#include "accounts-interface.h"
#include "accounts-user-interface.h"

#include <unistd.h>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#include <QList>
#include <QMutex>
#include <QScopedPointer>

AccountsGlobalInfo::AccountsGlobalInfo(QObject *parent)
    : QObject(parent),
      m_accountsInterface(QDBusConnection::systemBus())
{
}

AccountsGlobalInfo::~AccountsGlobalInfo()
{
}

AccountsGlobalInfo *AccountsGlobalInfo::instance()
{
    static QMutex                             mutex;
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
    connect(&m_accountsInterface, &AccountsInterface::UserAdded, [this](const QDBusObjectPath &user) {
        addUserToMap(user);
    });
    connect(&m_accountsInterface, &AccountsInterface::UserDeleted, [this](const QDBusObjectPath &user) {
        deleteUserFromMap(user);
    });

    ///加载账户
    QList<QDBusObjectPath>                    accounts;
    QDBusPendingReply<QList<QDBusObjectPath>> pendingReply;
    QList<QDBusObjectPath>                    objList;
    QList<QDBusObjectPath>::iterator          objListIter;
    pendingReply = m_accountsInterface.GetNonSystemUsers();
    pendingReply.waitForFinished();
    if (pendingReply.isError())
    {
        qWarning() << "GetNonSystemUsers Error:"
                   << pendingReply.error();
        return false;
    }
    objList = pendingReply.value();
    for (objListIter = objList.begin();
         objListIter != objList.end();
         objListIter++)
    {
        addUserToMap(*objListIter);
    }

    ///获取当前用户
    uid_t                              uid = getuid();
    QDBusPendingReply<QDBusObjectPath> findUserReply;
    findUserReply = m_accountsInterface.FindUserById(uid);
    findUserReply.waitForFinished();
    if (findUserReply.isError())
    {
        qWarning() << "FinduserById failed," << findUserReply.error();
    }
    else
    {
        UserInterface userInterface(findUserReply.value().path(), QDBusConnection::systemBus());
        m_curUserName = userInterface.user_name();
    }

    return true;
}

QList<QString> AccountsGlobalInfo::getUserList()
{
    QList<QString> userObjPathList;
    for (auto iter = m_usersList.begin();
         iter != m_usersList.end();
         iter++)
    {
        userObjPathList.append((*iter)->path());
    }
    return userObjPathList;
}

bool AccountsGlobalInfo::checkUserNameAvaliable(const QString &userName)
{
    bool isValid = true;

    for (auto &iter : m_usersList)
    {
        if (iter->user_name() == userName)
        {
            isValid = false;
            break;
        }
    }

    return isValid;
}

QString AccountsGlobalInfo::getCurrentUser()
{
    return m_curUserName;
}

void AccountsGlobalInfo::addUserToMap(const QDBusObjectPath &user)
{
    UserInterface *userInterface = new UserInterface(user.path(),
                                                     QDBusConnection::systemBus(),
                                                     this);
    connect(userInterface,
            &UserInterface::propertyChanged,
            this,
            &AccountsGlobalInfo::handlerPropertyChanged);
    m_usersList.append(userInterface);
    emit UserAdded(user);
}

void AccountsGlobalInfo::deleteUserFromMap(const QDBusObjectPath &user)
{
    int findIdx = -1;
    for (int i = 0; i < m_usersList.size(); i++)
    {
        if (m_usersList.at(i)->path() == user.path())
        {
            findIdx = i;
            break;
        }
    }
    if (findIdx == -1)
    {
        return;
    }
    UserInterface *interface = m_usersList.takeAt(findIdx);
    disconnect(interface,
               &UserInterface::propertyChanged,
               this,
               &AccountsGlobalInfo::handlerPropertyChanged);
    delete interface;
    emit UserDeleted(user);
}

///NOTE:暂时不用，考虑到侧边栏改变排序
void AccountsGlobalInfo::userListResort()
{
    std::sort(m_usersList.begin(), m_usersList.end(), [](const UserInterface *z1, const UserInterface *z2) {
        if (z1->locked() != z2->locked())
        {
            return !z1->locked();
        }
        else
        {
            return z1->user_name().toLower() < z2->user_name().toLower();
        }
    });
}

void AccountsGlobalInfo::handlerPropertyChanged(QString userPath, QString propertyName, QVariant value)
{
    emit UserPropertyChanged(userPath, propertyName, value);
}
