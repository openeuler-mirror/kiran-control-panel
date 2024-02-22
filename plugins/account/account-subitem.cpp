/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
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
#include "account-subitem.h"
#include "accounts-global-info.h"
#include "account-widget.h"

#define ACCOUNT_SEARCH_COMMAND_ADD_USER "@NewUser"
#define ACCOUNT_SEARCH_COMMAND_JUMP_TO_USER "@User:"

AccountSubItem::AccountSubItem(KiranControlPanel::PanelInterface* interface, QObject* parent)
    : QObject(parent),
      m_interface(interface)
{
    connect(AccountsGlobalInfo::instance(), &AccountsGlobalInfo::UserAdded, this, &AccountSubItem::handleUserAdded);
    connect(AccountsGlobalInfo::instance(), &AccountsGlobalInfo::UserDeleted, this, &AccountSubItem::handleUserDeleted);
}

AccountSubItem ::~AccountSubItem()
{
}

bool AccountSubItem::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_accountWidget && event->type() == QEvent::Destroy)
    {
        m_accountWidget = nullptr;
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
    searchKeys.append({tr("New User"), ACCOUNT_SEARCH_COMMAND_ADD_USER});

    QStringList users = AccountsGlobalInfo::instance()->getUserNameList();
    for (const QString& user : users)
    {
        searchKeys.append({user, QString("%1%2").arg(ACCOUNT_SEARCH_COMMAND_JUMP_TO_USER).arg(user)});
    }

    return searchKeys;
}

QWidget* AccountSubItem::createWidget()
{
    m_accountWidget = new AccountWidget();
    m_accountWidget->installEventFilter(this);
    return m_accountWidget;
}

bool AccountSubItem::jumpToSearchEntry(const QString& key)
{
    if (!m_accountWidget)
        return false;

    if (key.startsWith(ACCOUNT_SEARCH_COMMAND_ADD_USER))
    {
        m_accountWidget->jumpToAddUser();
    }
    else if (key.startsWith(ACCOUNT_SEARCH_COMMAND_JUMP_TO_USER))
    {
        QString keytemp = key;
        m_accountWidget->jumpToUser(keytemp.remove(ACCOUNT_SEARCH_COMMAND_JUMP_TO_USER));
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

void AccountSubItem::handleUserAdded(const QString& userObject)
{
    m_interface->handlePluginSubItemInfoChanged(getID());
}

void AccountSubItem::handleUserDeleted(const QString& userObject)
{
    m_interface->handlePluginSubItemInfoChanged(getID());
}