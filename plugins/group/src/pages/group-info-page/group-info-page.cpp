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
#include "group-info-page.h"
#include "./ui_group-info-page.h"
#include "accounts-global-info.h"
#include "def.h"
#include "ksd_group_admin_list_proxy.h"
#include "tools/group-name-checker.h"
#include "tools/group-name-validator.h"
#include "user-list-item.h"

#include <kiran-message-box.h>
#include <kiran-push-button.h>
#include <qt5-log-i.h>
#include <QKeyEvent>

enum PageEnum
{
    PAGE_GROUP_INFO,
    PAGE_ADD_USER
};

enum NameEditor
{
    NAME_LABEL,
    NAME_EDIT
};

GroupInfoPage::GroupInfoPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::GroupInfoPage)
{
    ui->setupUi(this);
    initUI();
}

GroupInfoPage::~GroupInfoPage()
{
    delete ui;
}

void GroupInfoPage::initUI()
{
    m_errorTip = new KiranTips(this);
    m_errorTip->setShowPosition(KiranTips::POSITION_BOTTM);
    m_errorTip->setAnimationEnable(true);

    // 所有用户名
    m_allUserName = getAllUserName();

    // 用户列表容器
    m_memberContainer = new UsersContainer(this);
    ui->layout_menber_list->addWidget(m_memberContainer);

    KiranPushButton::setButtonType(ui->btn_add_user, KiranPushButton::BUTTON_Default);
    KiranPushButton::setButtonType(ui->btn_delete, KiranPushButton::BUTTON_Warning);

    ui->btn_change_name->setIcon(QIcon::fromTheme("ksvg-rename"));
    ui->btn_change_name->setFocusPolicy(Qt::NoFocus);
    ui->btn_change_name->setStyleSheet("border:none;");

    // 用户组icon
    ui->avatar->setIcon(QIcon::fromTheme("krsvg-group-icon"));

    ui->stacked_name_editor->setCurrentIndex(NAME_LABEL);
    ui->edit_name->setValidator(new GroupNameValidator(ui->edit_name));
    ui->edit_name->installEventFilter(this);

    connect(ui->edit_name, &QLineEdit::returnPressed, this, &GroupInfoPage::onEditNameReturn);

    connect(ui->btn_change_name, &QPushButton::clicked, [this]()
            { 
                ui->edit_name->setFocus();
                ui->stacked_name_editor->setCurrentIndex(NAME_EDIT); });

    connect(ui->btn_add_user, &QPushButton::clicked, [this]()
            { emit requestAddUsersPage(m_curShowGroupPath); });

    connect(ui->btn_delete, &QPushButton::clicked, [this]()
            {
                ui->btn_delete->setBusy(true);
                emit requestDeleteGroup(m_gid, m_curShowGroupName); });
}

void GroupInfoPage::updateInfo()
{
    m_errorTip->hideTip();

    // 更新用户名列表
    m_allUserName = getAllUserName();

    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME,
                                      m_curShowGroupPath,
                                      QDBusConnection::systemBus());
    m_gid = groupProxy.gid();

    auto groupName = groupProxy.groupName();
    ui->label_name->setText(groupName);
    m_curShowGroupName = groupName;

    m_memberContainer->clear();
    /// 成员列表
    auto memberList = groupProxy.users();
    for (auto iter : memberList)
    {
        if (!iter.isEmpty())
        {
            appendMemberListItem(iter);
        }
    }
}

void GroupInfoPage::appendMemberListItem(const QString &userName)
{
    auto item = new UserListItem(m_memberContainer);
    item->setName(userName);
    item->setRightBtnIcon(QIcon::fromTheme("ksvg-trash"));
    item->setRightBtnVisible(true);
    m_memberContainer->addItem(item);

    connect(item, &UserListItem::rightButtonClicked, this, [this, item]()
            {
                ui->btn_add_user->setBusy(true);
                emit requestRemoveMember(m_curShowGroupPath, item->name()); });
}

QStringList GroupInfoPage::getAllUserName()
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

void GroupInfoPage::setCurrentShowGroupPath(const QString &groupObj)
{
    m_curShowGroupPath = groupObj;
    updateInfo();
    ui->stacked_name_editor->setCurrentIndex(NAME_LABEL);
}

void GroupInfoPage::onEditNameReturn()
{
    QString errorMessage;
    if (!GroupNameChecker::isValid(ui->edit_name->text(), errorMessage))
    {
        m_errorTip->setText(errorMessage);
        m_errorTip->showTipAroundWidget(ui->edit_name);
        return;
    }

    emit requestChangeGroupName(m_curShowGroupPath, ui->edit_name->text());
    ui->edit_name->clear();
}

void GroupInfoPage::onRemoveMemberFromGroupDone(QString errMsg)
{
    ui->btn_add_user->setBusy(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Ok);
        return;
    }
    updateInfo();
}

void GroupInfoPage::onAddUserToGroupDone(QString errMsg)
{
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Ok);
        return;
    }
    updateInfo();
}

void GroupInfoPage::onDeleteGroupDone(QString groupName, QString errMsg)
{
    ui->btn_delete->setBusy(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Ok);
    }
}

void GroupInfoPage::onChangeGroupNameDone(QString groupPath, QString errMsg)
{
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Ok);
    }
}

bool GroupInfoPage::eventFilter(QObject *watched, QEvent *event)
{
    if (ui->edit_name == watched)
    {
        if (event->type() == QEvent::FocusOut)
        {
            ui->edit_name->clear();
            ui->stacked_name_editor->setCurrentIndex(NAME_LABEL);
        }
        else if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape)
            {
                ui->stacked_name_editor->setCurrentIndex(NAME_LABEL);
                ui->edit_name->clear();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}
