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
#include "member-list-widget.h"

#include <QHBoxLayout>
#include <qt5-log-i.h>

MemberListWidget::MemberListWidget(QWidget *parent, QString userName):
QWidget(parent),userName(userName)
{
    initUI();
}

void MemberListWidget::initUI()
{
    userNameLabel = new QLabel(this->userName);
    QString deleteIconFile = QString(":/kcp-group-images/delete_member_icon.svg");
    deleteMemberButton = new QPushButton();
    deleteMemberButton->setIcon(QIcon(deleteIconFile));
    QHBoxLayout *HLayout = new QHBoxLayout;
    QSpacerItem *lSpacer = new QSpacerItem(10,40,QSizePolicy::Fixed);
    QSpacerItem *mSpacer = new QSpacerItem(10,40,QSizePolicy::Expanding);
    QSpacerItem *rSpacer = new QSpacerItem(24,40,QSizePolicy::Fixed);

    deleteMemberButton->setFocusPolicy(Qt::NoFocus);
    deleteMemberButton->setStyleSheet("border:none;");

    KLOG_INFO() << "adding widgets to list" << userName;

    HLayout->addItem(lSpacer);
    HLayout->addWidget(userNameLabel);
    HLayout->addItem(mSpacer);
    HLayout->addWidget(deleteMemberButton);
    HLayout->addItem(rSpacer);

    // 按下删除按钮发出信号
    connect(deleteMemberButton,&QPushButton::clicked,this,&MemberListWidget::handlerDeleteMemberButtonClicked);
    
    setLayout(HLayout);
}


QString MemberListWidget::getUserName()
{
    return userName;
}

void MemberListWidget::handlerDeleteMemberButtonClicked()
{
    emit sigDeleteMember(userName);
}