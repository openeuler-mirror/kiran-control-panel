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
#include "accounts-global-info.h"
#include "config.h"
#include "group-name-validator.h"
#include "groups-global-info.h"
#include "ksd_accounts_user_proxy.h"
#include "ui_create-group-page.h"
#include "user-list-widget.h"

#include <kiranwidgets-qt5/kiran-color-block.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <qt5-log-i.h>
// #include <widget-property-helper.h>

#define ITEM_USER_NAME_ROLE Qt::UserRole + 2

CreateGroupPage::CreateGroupPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::CreateGroupPage)
{
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
    ui->name_edit->setValidator(new GroupNameValidator(ui->name_edit));

    m_userContainter = new UsersContainer(this);
    ui->userListLayout->addWidget(m_userContainter);

    /// 初始用户选择
    QList<QString> userObjList;
    userObjList = AccountsGlobalInfo::instance()->getUserList();
    for (auto &iter : userObjList)
    {
        appendUserListItem(iter);
    }

    /// 确认按钮
    connect(ui->confirm_button, &QPushButton::clicked, this, &CreateGroupPage::handlerCreateNewGroup);

    // FIXME: 后续使用新版kiran-integration-qt5中提供的setButtonType函数
    // Kiran::StylePropertyHelper::setButtonType(ui->confirm_button, Kiran::BUTTON_Default);
}

void CreateGroupPage::appendUserListItem(const QString &userPath)
{
    KSDAccountsUserProxy interface(ACCOUNTS_DBUS_NAME, userPath, QDBusConnection::systemBus());

    UserListWidget *itemWidget = new UserListWidget(m_userContainter);
    QString chosenIconFile = QString(":/kcp-group-images/chosen_icon.svg");
    itemWidget->setRightButtonVisible(false, chosenIconFile);
    itemWidget->setText(interface.user_name());
    itemWidget->setClickable(true);
    connect(itemWidget, &UserListWidget::clicked, [itemWidget]()
            {
                if (!itemWidget->getRightButtionVisible())
                {
                    itemWidget->setRightButtonVisible(true, QString(":/kcp-group-images/chosen_icon.svg"));
                }
                else
                {
                    itemWidget->setRightButtonVisible(false, QString(":/kcp-group-images/chosen_icon.svg"));
                } });
    m_userContainter->addFeatureItem(itemWidget);
}

void CreateGroupPage::handlerCreateNewGroup()
{
    // 检验用户组名是否为空，是否重名
    KLOG_INFO() << "start check group name";
    QString groupName = ui->name_edit->text();

    if (groupName.isEmpty())
    {
        m_errorTip->setText(tr("Please enter your group name"));
        m_errorTip->showTipAroundWidget(ui->name_edit);
        return;
    }

    bool isPureDigital = true;
    for (QChar ch : groupName)
    {
        if (!ch.isNumber())
        {
            isPureDigital = false;
            break;
        }
    }

    if (isPureDigital)
    {
        m_errorTip->setText(tr("group name cannot be a pure number"));
        m_errorTip->showTipAroundWidget(ui->name_edit);
        return;
    }

    if (!GroupsGlobalInfo::instance()->checkGroupNameAvaliable(groupName))
    {
        m_errorTip->setText(tr("group name already exists"));
        m_errorTip->showTipAroundWidget(ui->name_edit);
        return;
    }

    emit sigIsBusyChanged(true);
    ui->confirm_button->setBusy(true);
    emit sigCreateGroup(groupName);
}

void CreateGroupPage::handlerCreateNewGroupIsDone(QString groupPath, QString errMsg)
{
    ui->confirm_button->setBusy(false);
    emit sigIsBusyChanged(false);
    QStringList userNameList;

    auto userList = m_userContainter->getAllFeatureItem();
    for (auto &item : userList)
    {
        auto userListWidget = qobject_cast<UserListWidget *>(item);
        if (userListWidget->getRightButtionVisible())
        {
            userNameList.append(userListWidget->getText());
        }
    }

    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Yes | KiranMessageBox::No);
    }

    if (!groupPath.isEmpty())
    {
        ui->confirm_button->setBusy(true);
        emit sigIsBusyChanged(true);
        //        emit sigRequestSetCurrentGroup(groupPath);
        emit sigAddUserToGroup(groupPath, userNameList);
    }
}

void CreateGroupPage::handlerAddUserToGroupIsDone(QString errMsg)
{
    emit sigIsBusyChanged(false);
    ui->confirm_button->setBusy(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Yes | KiranMessageBox::No);
    }
    reset();
}

void CreateGroupPage::reset()
{
    ui->name_edit->clear();
    m_userContainter->clear();
    QList<QString> userObjList;
    userObjList = AccountsGlobalInfo::instance()->getUserList();
    for (auto &iter : userObjList)
    {
        appendUserListItem(iter);
    }
    m_errorTip->hideTip();
}
