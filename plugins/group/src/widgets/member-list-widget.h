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
#ifndef KIRAN_MEMBER_LIST_WIDGET_H
#define KIRAN_MEMBER_LIST_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class MemberListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemberListWidget(QWidget *parent, QString userName);
    ~MemberListWidget() = default;

    void initUI();
    QString getUserName();

signals:
    void sigDeleteMember(QString userName);

private Q_SLOTS:
    void handlerDeleteMemberButtonClicked();


private:
    QString userName = "unknown";
    QLabel *userNameLabel;
    QPushButton *deleteMemberButton;
};

#endif