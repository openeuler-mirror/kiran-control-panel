/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
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
