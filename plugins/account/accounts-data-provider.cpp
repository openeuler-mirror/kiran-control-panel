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
#include "accounts-data-provider.h"
#include "config.h"
#include "logging-category.h"
#include "user-loader.h"

#include <kiran-system-daemon/accounts-i.h>
#include <qt5-log-i.h>
#include <unistd.h>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#include <QList>
#include <QSettings>
#include <QThread>

AccountsDataProvider::AccountsDataProvider(QObject *parent)
    : QObject(parent),
      m_accountsInterface(DBusWrapper::createKiranAccountServiceAPI())
{
    if (m_accountsInterface.isNull())
    {
        KLOG_ERROR(qLcAccount) << "create accounts service interface failed!";
        return;
    }

    connect(m_accountsInterface.data(), &KiranAccountService::UserAdded,
            this, &AccountsDataProvider::handleUserAdded);
    connect(m_accountsInterface.data(), &KiranAccountService::UserDeleted,
            this, &AccountsDataProvider::handleUserDeleted);
    connect(m_accountsInterface.data(), &KiranAccountService::rsa_public_keyChanged,
            [this](const QString &publicKey)
            { m_pubkey = publicKey; });

    // 属性变化只在服务上订阅一次：空对象路径表示该服务下的所有对象，
    // arg0(发生变化的接口名)过滤到用户接口，槽里再按消息里的对象路径区分是哪个用户。
    // 不能每个用户订阅一次：一个用户一条 match rule，用户数上千时会撞上 dbus-daemon
    // 的规则数上限(默认512)，超出的订阅会静默失效
    QDBusConnection::systemBus().connect(
        DBusWrapper::KiranAccountServiceName,
        QString(),
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("PropertiesChanged"),
        QStringList() << QString::fromLatin1(KiranAccountServiceUser::staticInterfaceName()),
        QStringLiteral("sa{sv}as"),
        this, SLOT(handleDbusPropertiesChanged(QDBusMessage)));

    // 判断是否显示ROOT用户
    QSettings settings(SETTINGS_PATH, QSettings::IniFormat);
    if (settings.status() != QSettings::NoError)
    {
        KLOG_WARNING(qLcAccount) << "parse" << SETTINGS_PATH << "failed!" << settings.status();
    }
    else
    {
        m_showRoot = settings.value("Account/ShowRoot", false).toBool();
    };
    KLOG_DEBUG(qLcAccount, "show root:%s", m_showRoot ? "true" : "false");
}

AccountsDataProvider::~AccountsDataProvider()
{
    stopBackgroundLoad();
}

void AccountsDataProvider::loadUsers()
{
    if (m_accountsInterface.isNull())
    {
        return;
    }

    // 加载用户
    QDBusPendingReply<QList<QDBusObjectPath>> getUsersReply = m_accountsInterface->GetNonSystemUsers();
    getUsersReply.waitForFinished();
    if (getUsersReply.isError())
    {
        KLOG_ERROR(qLcAccount) << "get non system users error:"
                               << getUsersReply.error();
        return;
    }
    QList<QDBusObjectPath> objList = getUsersReply.value();

    if (m_showRoot)
    {
        // 将Root用户加入链表中
        auto getRootReply = m_accountsInterface->FindUserById(0);
        getRootReply.waitForFinished();
        if (getRootReply.isError())
        {
            KLOG_ERROR(qLcAccount) << "insert root to users cache failed,"
                                   << "cant find root by id:" << getRootReply.error();
        }
        else
        {
            objList.insert(0, getRootReply.value());
        }
    }

    const QString currentUserPath = prepareCurrentUser(objList);

    m_pubkey = m_accountsInterface->rsa_public_key();

    QStringList allPaths;
    for (auto iter = objList.begin(); iter != objList.end(); ++iter)
    {
        allPaths.append(iter->path());
    }
    allPaths.removeOne(currentUserPath);

    if (!allPaths.isEmpty())
    {
        KLOG_DEBUG(qLcAccount) << "load" << allPaths.size() << "users in background";
        startBackgroundLoad(allPaths);
    }
}

QString AccountsDataProvider::prepareCurrentUser(const QList<QDBusObjectPath> &userList)
{
    auto findUserReply = m_accountsInterface->FindUserById(getuid());
    findUserReply.waitForFinished();
    if (findUserReply.isError())
    {
        KLOG_WARNING(qLcAccount) << "find current user failed!";
        return QString();
    }

    const QString currentUserPath = findUserReply.value().path();
    const QVariantMap values = UserLoader::fetchUserValues(currentUserPath);
    m_curUserName = values.value(QStringLiteral("user_name")).toString();
    KLOG_DEBUG(qLcAccount) << "current user:" << m_curUserName;

    // 当前用户不一定在用户列表里（例如未开启 ShowRoot 时的 root），只在会显示时才下发
    if (userList.contains(QDBusObjectPath(currentUserPath)) && !values.isEmpty())
    {
        emitUserAdded(currentUserPath, values);
    }

    return currentUserPath;
}

bool AccountsDataProvider::checkUserNameAvaliable(const QString &userName)
{
    if (m_accountsInterface.isNull())
    {
        return true;
    }

    // 直接问后端这个名字在不在，不再遍历本地所有用户逐个查D-Bus属性：
    // 之前每多一个用户就多一次阻塞往返，用户多的时候点"创建用户"会把界面卡住
    QDBusPendingReply<QDBusObjectPath> findUserReply = m_accountsInterface->FindUserByName(userName);
    findUserReply.waitForFinished();
    if (findUserReply.isError())
    {
        // 后端用错误回复表示没有发现该用户
        KLOG_DEBUG(qLcAccount) << "find user by name failed:" << findUserReply.error();
        return true;
    }

    return false;
}

QString AccountsDataProvider::getCurrentUser()
{
    return m_curUserName;
}

QString AccountsDataProvider::rsaPublicKey()
{
    return m_pubkey;
}

void AccountsDataProvider::handleUserAdded(const QDBusObjectPath &user)
{
    // 新增用户的展示值也放到加载器线程加载，不在主线程等D-Bus返回
    startBackgroundLoad(QStringList() << user.path());
}

void AccountsDataProvider::handleUserDeleted(const QDBusObjectPath &user)
{
    KLOG_DEBUG(qLcAccount) << "delete user:" << user.path();
    emit UserDeleted(user.path());
}

void AccountsDataProvider::handleUserLoaded(const QString &objectPath, const QVariantMap &values)
{
    emitUserAdded(objectPath, values);
}

void AccountsDataProvider::emitUserAdded(const QString &objectPath, const QVariantMap &values)
{
    emit UserAdded(objectPath,
                   values.value(QStringLiteral("user_name")).toString(),
                   values.value(QStringLiteral("icon_file")).toString(),
                   values.value(QStringLiteral("locked")).toBool());
}

void AccountsDataProvider::handleDbusPropertiesChanged(const QDBusMessage &message)
{
    const QList<QVariant> args = message.arguments();
    if (args.count() != 3)
    {
        return;
    }

    // 订阅规则里已经按arg0过滤过，这里再确认一次，避免误处理其它接口的属性变化
    if (args.at(0).toString() != QString::fromLatin1(KiranAccountServiceUser::staticInterfaceName()))
    {
        return;
    }

    const QVariantMap changedProperties = qdbus_cast<QVariantMap>(args.at(1).value<QDBusArgument>());
    for (auto iter = changedProperties.begin(); iter != changedProperties.end(); iter++)
    {
        KLOG_DEBUG(qLcAccount) << "user property changed:"
                               << " user:" << message.path()
                               << " property name:" << iter.key()
                               << " value:" << iter.value();
        emit UserPropertyChanged(message.path(), iter.key(), iter.value());
    }
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
        KLOG_WARNING(qLcAccount) << "user loader did not stop in 10s, detach it";
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
