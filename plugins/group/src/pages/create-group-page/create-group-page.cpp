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

#include "create-group-page.h"
#include "accounts-data-provider.h"
#include "group-manager.h"
#include "group-name-checker.h"
#include "group-name-validator.h"
#include "kiran-tips/kiran-tips.h"
#include "ui_create-group-page.h"
#include "user-list-item.h"
#include "users-container.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <style-property.h>
#include <QTimer>

using namespace Kiran;

CreateGroupPage::CreateGroupPage(AccountsDataProvider *accountsDataProvider, QWidget *parent)
    : QWidget(parent), ui(new Ui::CreateGroupPage)
{
    m_accountsDataProvider = accountsDataProvider;

    ui->setupUi(this);

    initUI();
}

CreateGroupPage::~CreateGroupPage()
{
    delete ui;
}

void CreateGroupPage::initUI()
{
    /// 提示框
    m_errorTip = new KiranTips(this);
    m_errorTip->setShowPosition(KiranTips::POSITION_BOTTM);
    m_errorTip->setAnimationEnable(true);

    /// 用户组名输入框
    ui->edit_name->setValidator(new GroupNameValidator(ui->edit_name));

    // 用户列表
    m_userContainter = new UsersContainer(this);
    ui->userListLayout->addWidget(m_userContainter);

    connect(m_accountsDataProvider, &AccountsDataProvider::UserAdded,
            this, &CreateGroupPage::appendUser);

    /// 确认按钮
    StylePropertyHelper::setButtonType(ui->btn_confirm, BUTTON_Default);
    connect(ui->btn_confirm, &QPushButton::clicked, this, &CreateGroupPage::createGroup);
}

void CreateGroupPage::appendUser(const QDBusObjectPath &objectPath, const QString &userName)
{
    Q_UNUSED(objectPath);

    m_allUserName.append(userName);
    m_pendingUserNames.append(userName);
    scheduleAppend();
}

void CreateGroupPage::scheduleAppend()
{
    if (m_appendPending)
    {
        return;
    }

    m_appendPending = true;
    QTimer::singleShot(0, this, &CreateGroupPage::appendPendingUsers);
}

void CreateGroupPage::appendPendingUsers()
{
    m_appendPending = false;

    // 每次只补入一部分，避免一次建出上万个条目把界面卡住
    int count = 0;
    while (!m_pendingUserNames.isEmpty() && count < 100)
    {
        appendUserListItem(m_pendingUserNames.takeFirst());
        count++;
    }

    // 一批加完统一重算一次高度，逐条重算会把布局反复作废（O(N^2)）
    if (count > 0)
    {
        m_userContainter->adjustSizeToItmes();
    }

    if (!m_pendingUserNames.isEmpty())
    {
        scheduleAppend();
    }
}

void CreateGroupPage::appendUserListItem(const QString &userName)
{
    auto item = new UserListItem(m_userContainter);
    item->setRightBtnIcon(QIcon(":/kcp-group-images/chosen_icon.svg"));
    item->setRightBtnVisible(false);
    item->setName(userName);
    item->setClickable(true);
    m_userContainter->addItem(item);
}

void CreateGroupPage::createGroup()
{
    QString errorMessage;
    auto groupName = ui->edit_name->text();
    if (!GroupNameChecker::isValid(groupName, errorMessage))
    {
        m_errorTip->setText(errorMessage);
        m_errorTip->showTipAroundWidget(ui->edit_name);
        return;
    }

    // 添加用户到用户组
    QStringList userNameList;
    auto items = m_userContainter->getSelectedItems();
    for (auto item : items)
    {
        userNameList.append(item->name());
    }

    ui->btn_confirm->setBusy(true);
    emit requestCreateGroup(groupName, userNameList);
}

void CreateGroupPage::handleGroupAdded(const QString &groupPath, const QString &errMsg)
{
    Q_UNUSED(groupPath);
    ui->btn_confirm->setBusy(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Ok);
    }
}

void CreateGroupPage::reset()
{
    ui->edit_name->clear();
    m_userContainter->clear();
    m_pendingUserNames = m_allUserName;
    scheduleAppend();
    m_errorTip->hideTip();
}
