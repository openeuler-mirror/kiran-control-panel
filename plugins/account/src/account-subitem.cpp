#include "account-subitem.h"
#include "accounts-global-info.h"
#include "kiran-account-manager.h"

AccountSubItem::AccountSubItem(KiranControlPanel::PanelInterface* interface, QObject* parent)
    : m_interface(interface),
      QObject(parent)
{
    connect(AccountsGlobalInfo::instance(), &AccountsGlobalInfo::UserAdded, this, &AccountSubItem::handleUserAdded);
    connect(AccountsGlobalInfo::instance(), &AccountsGlobalInfo::UserDeleted, this, &AccountSubItem::handleUserDeleted);
}

AccountSubItem ::~AccountSubItem()
{
}

bool AccountSubItem::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_subitemWidget && event->type() == QEvent::Destroy)
    {
        m_subitemWidget = nullptr;
    }
    return QObject::eventFilter(watched, event);
}

QString AccountSubItem::getID()
{
    return "Account";
}

QString AccountSubItem::getName()
{
    return tr("account");
}

QString AccountSubItem::getCategory()
{
    return "account-management";
}

QString AccountSubItem::getDesc()
{
    return "";
}

QString AccountSubItem::getIcon()
{
    return "kcp-account";
}

int AccountSubItem::getWeight()
{
    return 999;
}

QVector<QPair<QString, QString>> AccountSubItem::getSearchKeys()
{
    QVector<QPair<QString, QString>> searchKeys;
    searchKeys.append({tr("New User"), "@NewUser"});

    QStringList users = AccountsGlobalInfo::instance()->getUserNameList();
    for (const QString& user : users)
    {
        searchKeys.append({user, QString("@User:%1").arg(user)});
    }

    return searchKeys;
}

QWidget* AccountSubItem::createWidget()
{
    m_subitemWidget = new KiranAccountManager();
    m_subitemWidget->installEventFilter(this);
    return m_subitemWidget;
}

bool AccountSubItem::jumpToSearchEntry(const QString& key)
{
    if (!m_subitemWidget)
        return false;

    if (key.startsWith("@NewUser"))
    {
        m_subitemWidget->jumpToAddUser();
    }
    else if (key.startsWith("@User:"))
    {
        QString keytemp = key;
        m_subitemWidget->jumpToUser(keytemp.remove("@User:"));
    }
    else
    {
        return false;
    }
    return true;
}

bool AccountSubItem::haveUnSavedOptions()
{
    return false;
}

void AccountSubItem::handleUserAdded(const QDBusObjectPath& userObject)
{
    m_interface->handlePluginSubItemInfoChanged(getID());
}

void AccountSubItem::handleUserDeleted(const QDBusObjectPath& userObject)
{
    m_interface->handlePluginSubItemInfoChanged(getID());
}