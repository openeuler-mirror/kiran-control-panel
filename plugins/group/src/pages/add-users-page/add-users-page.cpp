/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "add-users-page.h"
#include "./ui_add-users-page.h"
#include "def.h"
#include "ksd_group_admin_list_proxy.h"
#include "src/accounts-global-info.h"
#include "user-list-item.h"

#include <kiran-message-box.h>
#include <kiran-push-button.h>
#include <qt5-log-i.h>

AddUsersPage::AddUsersPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::AddUsersPage)
{
    ui->setupUi(this);
    initUI();
}

AddUsersPage::~AddUsersPage()
{
    delete ui;
}

void AddUsersPage::initUI()
{
    // 所有用户名
    m_allUserName = getAllUserName();

    // 用户列表容器
    m_usersContainer = new UsersContainer(this);
    ui->layout_user_list->addWidget(m_usersContainer);

    KiranPushButton::setButtonType(ui->btn_save, KiranPushButton::BUTTON_Default);

    ui->search_box->setPlaceholderText(tr("Please input keys for search..."));
    /// 连接搜索框和user_list，使搜索框输入文字时在user_list上显示匹配到的用户
    connect(ui->search_box, &QLineEdit::textEdited, this, &AddUsersPage::searchFilter);

    connect(ui->btn_cancel, &QPushButton::clicked, [this]()
            { emit requestGroupInfoPage(); });

    connect(ui->btn_save, &QPushButton::clicked, [this]()
            {
                QStringList userNameList;
                for (auto item : m_usersContainer->getSelectedItems())
                {
                    userNameList.append(item->name());
                }
                if (!userNameList.isEmpty())
                {
                    ui->btn_save->setBusy(true);
                    emit requestAddUserToGroup(m_curShowGroupPath, userNameList);} });
}

QStringList AddUsersPage::getAllUserName()
{
    QStringList result;
    auto userObjList = AccountsGlobalInfo::instance()->getUserList();
    for (auto iter : userObjList)
    {
        KSDAccountsUserProxy interface(ACCOUNTS_DBUS_NAME, iter, QDBusConnection::systemBus());
        auto userName = interface.user_name();
        result.append(userName);
    }
    return result;
}

void AddUsersPage::updateUsersList(const QString &groupObj)
{
    m_curShowGroupPath = groupObj;

    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME,
                                      m_curShowGroupPath,
                                      QDBusConnection::systemBus());
    m_usersContainer->clear();
    /// 加载不在用户组中的用户
    auto usersInGroup = groupProxy.users();
    for (auto name : m_allUserName)
    {
        if (!usersInGroup.contains(name))
        {
            appendUserListItem(name);
        }
    }

    ui->search_box->setFocus();
}

void AddUsersPage::appendUserListItem(const QString &userName)
{
    auto item = new UserListItem(m_usersContainer);
    item->setName(userName);
    item->setClickable(true);
    item->setRightBtnIcon(QIcon(":/kcp-group-images/chosen_icon.svg"));
    item->setRightBtnVisible(false);
    m_usersContainer->addItem(item);
}

void AddUsersPage::searchFilter(QString filterString)
{
    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME,
                                      m_curShowGroupPath,
                                      QDBusConnection::systemBus());
    auto usersInGroup = groupProxy.users();

    if (filterString.size() == 0)
    {
        /// 加载不在用户组中的用户
        for (auto name : m_allUserName)
        {
            if (!usersInGroup.contains(name))
            {
                m_usersContainer->setItemVisible(name, true);
            }
        }
    }
    else
    {
        for (auto name : m_allUserName)
        {
            if (!usersInGroup.contains(name))
            {
                // 显示/隐藏过滤后的用户
                if (name.contains(filterString))
                {
                    m_usersContainer->setItemVisible(name, true);
                }
                else
                {
                    m_usersContainer->setItemVisible(name, false);
                }
                continue;
            }
        }
    }
}

void AddUsersPage::onAddUserToGroupDone(QString errMsg)
{
    ui->btn_save->setBusy(false);
    emit requestGroupInfoPage();
}