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

#include "account-itemwidget.h"
#include "ksd_accounts_user_proxy.h"
#include "ui_account-itemwidget.h"
#include <kiran-system-daemon/accounts-i.h>
#include <QStyle>

AccountItemWidget::AccountItemWidget(QWidget *paren, bool isCreateAccountItem) : QWidget(paren),
                                                                                 ui(new Ui::AccountItemWidget),
                                                                                 m_isSelected(false),
                                                                                 m_isCreateAccountItem(isCreateAccountItem),
                                                                                 m_isLocked(false)
{
    ui->setupUi(this);
    if (m_isCreateAccountItem)
    {
        ui->avatar->setImage(":/kcp-account/images/create-user-avatar.png");
        ui->label_userName->setText(tr("Create new user"));
        ui->label_status->setVisible(false);
    }
    updateStatusDesc();
}

AccountItemWidget::~AccountItemWidget()
{
    delete ui;
}

bool AccountItemWidget::isSelected() const
{
    return m_isSelected;
}

bool AccountItemWidget::isCreateAccountItem() const
{
    return m_isCreateAccountItem;
}

bool AccountItemWidget::isLocked() const
{
    return m_isLocked;
}

void AccountItemWidget::setUserDBusObjectPath(const QString &objPath)
{
    m_objectPath = objPath;
    updateInfo();
}

QString AccountItemWidget::getUserDBusObjectPath()
{
    return m_objectPath;
}

void AccountItemWidget::updateInfo()
{
    KSDAccountsUserProxy userProxy(ACCOUNTS_DBUS_NAME,m_objectPath, QDBusConnection::systemBus());
    ui->label_userName->setText(userProxy.user_name());
    setIsLocked(userProxy.locked());
    ui->avatar->setImage(userProxy.icon_file());
}

void AccountItemWidget::setSelected(bool isSelected)
{
    if (m_isSelected == isSelected)
    {
        return;
    }
    m_isSelected = isSelected;
    style()->polish(ui->indicator);
    style()->polish(ui->label_status);
    emit isSelectedChanged(m_isSelected);
}

void AccountItemWidget::setIsLocked(bool isLocked)
{
    if (m_isLocked == isLocked)
    {
        return;
    }
    m_isLocked = isLocked;
    updateStatusDesc();
    style()->polish(ui->label_status);
}

void AccountItemWidget::updateStatusDesc()
{
    if (m_isLocked)
    {
        ui->label_status->setText(tr("disable"));
    }
    else
    {
        ui->label_status->setText(tr("enable"));
    }
}
