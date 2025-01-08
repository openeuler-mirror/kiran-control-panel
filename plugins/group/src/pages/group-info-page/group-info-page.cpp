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
#include <palette.h>
#include <style-helper.h>
#include "./ui_group-info-page.h"
#include "accounts-global-info.h"
#include "config.h"
#include "ksd_group_admin_list_proxy.h"
#include "member-list-widget.h"
#include "user-list-widget.h"

#include <kiranwidgets-qt5/kiran-message-box.h>
#include <qt5-log-i.h>
#include <QAction>
#include <QKeyEvent>
#include <QPainter>

using namespace Kiran::Theme;

enum PageEnum
{
    PAGE_GROUP_INFO,
    PAGE_ADD_USER
};

enum NameEdit
{
    NAME_LABEL,
    LINE_EDIT
};

#define ITEM_USER_NAME_ROLE Qt::UserRole + 2

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

    m_memberContainer = new UsersContainer(ui->page_groupInfo);
    ui->layoutMenberList->addWidget(m_memberContainer);
    m_usersContainer = new UsersContainer(ui->page_addUser);
    ui->layoutUserList->addWidget(m_usersContainer);

    // FIXME: 后续使用新版kiran-integration-qt5中提供的setButtonType函数
    // Kiran::StylePropertyHelper::setButtonType(ui->add_member_button, Kiran::BUTTON_Default);
    // Kiran::StylePropertyHelper::setButtonType(ui->confirm_add_button, Kiran::BUTTON_Default);

    connect(ui->add_member_button, &QPushButton::clicked, [this]()
            { ui->stackedWidget->setCurrentIndex(PAGE_ADD_USER); });

    connect(ui->cancel_button, &QPushButton::clicked, [this]()
            {
                m_errorTip->hideTip();
                ui->stackedWidget->setCurrentIndex(PAGE_GROUP_INFO); });

    // 用户组icon
    QString groupIconFile = QString(":/kcp-group-images/group_icon_large.png");
    QImage *iconImage = new QImage;
    iconImage->load(groupIconFile);
    ui->avatar->setPixmap(QPixmap::fromImage(*iconImage));

    QString changeNameIconFile = QString(":/kcp-group-images/change_name_icon.svg");
    ui->change_name_button->setIcon(QIcon(changeNameIconFile));
    ui->change_name_button->setFocusPolicy(Qt::NoFocus);
    ui->change_name_button->setStyleSheet("border:none;");

    updateIcon();
    connect(DEFAULT_PALETTE(), &Palette::baseColorsChanged, this, &GroupInfoPage::updateIcon);

    ui->stackedWidget_edit_name->setCurrentIndex(NAME_LABEL);

    connect(ui->delete_button, &QPushButton::clicked, [this]()
            {
                //                emit sigIsBusyChanged(true);
                ui->delete_button->setBusy(true);
                emit sigDeleteGroup(gid, m_curShowGroupName); });

    connect(ui->change_name_button, &QPushButton::clicked, [this]()
            { ui->stackedWidget_edit_name->setCurrentIndex(LINE_EDIT); });

    ui->lineEdit->installEventFilter(this);

    /// 连接搜索框和user_list，使搜索框输入文字时在user_list上显示匹配到的用户
    connect(ui->search_box, &QLineEdit::textEdited, this, &GroupInfoPage::searchFilter);

    QPixmap in(":/kcp-group-images/search.svg");
    QPixmap out(in.size() * 10 / 5);
    QRect r = in.rect();
    r.moveCenter(out.rect().center());
    out.fill(Qt::transparent);

    QPainter painter(&out);
    painter.drawPixmap(r, in);
    painter.end();
    QIcon searchIcon(out);
    QAction *action = new QAction(this);
    action->setIcon(searchIcon);
    ui->search_box->addAction(action, QLineEdit::LeadingPosition);

    ui->search_box->setPlaceholderText(tr("Please input keys for search..."));

    connect(ui->confirm_add_button, &QPushButton::clicked, [this]()
            {
                //                emit sigIsBusyChanged(true);
                ui->confirm_add_button->setBusy(true);
                QStringList userNameList;

                for (auto &item : m_usersContainer->getAllFeatureItem())
                {
                    auto userListWidget = qobject_cast<UserListWidget *>(item);
                    if (userListWidget->getRightButtionVisible())
                    {
                        userNameList.append(userListWidget->getText());
                    }
                }
                emit sigAddUserToGroup(m_curShowGroupPath, userNameList); });
}

void GroupInfoPage::appendMemberListItem(const QString &userName)
{
    UserListWidget *itemWidget = new UserListWidget(m_memberContainer);
    itemWidget->setText(userName);
    itemWidget->setUserData(userName);
    itemWidget->setRightButtonVisible(true, QString(":/kcp-group-images/delete_member_icon.svg"));

    connect(itemWidget, &UserListWidget::rightButtonClicked, [this](const QVariant &userName)
            {
                //                emit sigIsBusyChanged(true);
                ui->add_member_button->setBusy(true);
                emit sigRemoveMember(m_curShowGroupPath, userName.toString()); });

    m_memberContainer->addFeatureItem(itemWidget);
}

void GroupInfoPage::updateInfo()
{
    m_errorTip->hideTip();

    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME,
                                      m_curShowGroupPath,
                                      QDBusConnection::systemBus());

    QString groupName = groupProxy.groupName();

    gid = groupProxy.gid();

    ui->name_label->setText(groupName);

    m_curShowGroupName = groupName;

    ui->stackedWidget->setCurrentIndex(PAGE_GROUP_INFO);

    m_memberContainer->clear();

    /// 成员列表
    QList<QString> memberList;
    memberList = groupProxy.users();

    for (auto &iter : memberList)
    {
        appendMemberListItem(iter);
    }

    m_usersContainer->clear();

    /// 加载不在用户组中的用户
    QStringList usersInGroup = groupProxy.users();
    QStringList userObjList;
    userObjList = AccountsGlobalInfo::instance()->getUserList();
    for (auto &iter : userObjList)
    {
        auto userProxy = new KSDAccountsUserProxy(ACCOUNTS_DBUS_NAME,
                                                  iter,
                                                  QDBusConnection::systemBus(),
                                                  this);
        QString userName = userProxy->user_name();
        if (usersInGroup.indexOf(userName) == -1)
        {
            appendUserListItem(iter);
        }
    }
}

void GroupInfoPage::appendUserListItem(const QString &userPath)
{
    KSDAccountsUserProxy interface(ACCOUNTS_DBUS_NAME, userPath, QDBusConnection::systemBus());

    auto item = new UserListWidget(m_usersContainer);
    item->setText(interface.user_name());
    item->setClickable(true);
    item->setUserData(interface.user_name());
    item->setRightButtonVisible(false, QString(":/kcp-group-images/chosen_icon.svg"));

    connect(item, &UserListWidget::clicked, [item]()
            {
                if (!item->getRightButtionVisible())
                {
                    item->setRightButtonVisible(true, QString(":/kcp-group-images/chosen_icon.svg"));
                }
                else
                {
                    item->setRightButtonVisible(false, QString(":/kcp-group-images/chosen_icon.svg"));
                } });

    m_usersContainer->addFeatureItem(item);
}

void GroupInfoPage::setCurrentShowGroupPath(const QString &groupObj)
{
    m_curShowGroupPath = groupObj;
    updateInfo();
    ui->stackedWidget_edit_name->setCurrentIndex(NAME_LABEL);
}

void GroupInfoPage::handlerRemoveMemberIsDone(QString errMsg)
{
    //    emit sigIsBusyChanged(false);
    ui->add_member_button->setBusy(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Yes | KiranMessageBox::No);
    }
    updateInfo();
}

void GroupInfoPage::handlerDeleteGroupIsDone(QString groupName, QString errMsg)
{
    //    emit sigIsBusyChanged(false);
    ui->delete_button->setBusy(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Yes | KiranMessageBox::No);
    }
}

void GroupInfoPage::handlerChangeGroupNameIsDone(QString groupPath, QString errMsg)
{
    //    emit sigIsBusyChanged(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Yes | KiranMessageBox::No);
    }
}

void GroupInfoPage::handlerAddUserToGroupIsDone(QString errMsg)
{
    //    emit sigIsBusyChanged(false);
    ui->confirm_add_button->setBusy(false);
    if (!errMsg.isEmpty())
    {
        KiranMessageBox::message(nullptr, tr("Error"),
                                 errMsg, KiranMessageBox::Yes | KiranMessageBox::No);
    }
    updateInfo();
    ui->stackedWidget->setCurrentIndex(PAGE_GROUP_INFO);
}

bool GroupInfoPage::eventFilter(QObject *watched, QEvent *event)
{
    if (ui->lineEdit == watched)
    {
        if (event->type() == QEvent::FocusOut)
        {
            ui->lineEdit->clear();
            ui->stackedWidget_edit_name->setCurrentIndex(NAME_LABEL);
        }
        else if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if ((keyEvent->key() == Qt::Key_Return) ||
                (keyEvent->key() == Qt::Key_Enter))
            {
                //                emit sigIsBusyChanged(false);
                emit sigChangeGroupName(m_curShowGroupPath, ui->lineEdit->text());
                ui->lineEdit->clear();
            }
            if (keyEvent->key() == Qt::Key_Escape)
            {
                ui->lineEdit->clear();
                ui->stackedWidget_edit_name->setCurrentIndex(NAME_LABEL);
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void GroupInfoPage::searchFilter(QString filterString)
{
    KSDGroupAdminListProxy groupProxy(GROUP_ADMIN_DBUS_NAME,
                                      m_curShowGroupPath,
                                      QDBusConnection::systemBus());
    QStringList usersInGroup = groupProxy.users();
    QStringList userObjList;
    userObjList = AccountsGlobalInfo::instance()->getUserList();
    m_usersContainer->clear();

    if (filterString.size() == 0)
    {
        /// 加载不在用户组中的用户
        for (auto &iter : userObjList)
        {
            auto userProxy = new KSDAccountsUserProxy(ACCOUNTS_DBUS_NAME,
                                                      iter,
                                                      QDBusConnection::systemBus(),
                                                      this);
            QString userName = userProxy->user_name();
            if (usersInGroup.indexOf(userName) == -1)
            {
                appendUserListItem(iter);
            }
        }
    }
    else
    {
        for (auto &iter : userObjList)
        {
            auto userProxy = new KSDAccountsUserProxy(ACCOUNTS_DBUS_NAME,
                                                      iter,
                                                      QDBusConnection::systemBus(),
                                                      this);
            QString userName = userProxy->user_name();
            if (usersInGroup.indexOf(userName) == -1)
            {
                if (userName.indexOf(filterString) != -1)
                {
                    appendUserListItem(iter);
                    continue;
                }
            }
        }
    }
}

void GroupInfoPage::updateIcon()
{
    QIcon groupIcon(":/kcp-group-images/group_icon_large.png");
    QPixmap groupPixmap = groupIcon.pixmap(90, 90);

    if (DEFAULT_STYLE_HELPER()->paletteType() != PaletteType::PALETTE_DARK)
    {
        QImage groupImage = groupPixmap.toImage();
        groupImage.invertPixels(QImage::InvertRgb);
        groupPixmap = QPixmap::fromImage(groupImage);
    }

    ui->avatar->setPixmap(groupPixmap);
}
