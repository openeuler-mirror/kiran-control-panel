/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */
#include "active-list-item.h"
#include "ui_active-list-item.h"

ActListItem::ActListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActListItem)
{
    ui->setupUi(this);
    iniUI();
}

ActListItem::~ActListItem()
{
    delete ui;
}

void ActListItem::iniUI()
{

}

void ActListItem::setItemText(const QString &text)
{
    ui->label_item_name->setText(text);
}
