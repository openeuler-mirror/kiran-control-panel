/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keyboard is licensed under Mulan PSL v2.
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

#include "choose-item.h"
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "ui_choose-item.h"
ChooseItem::ChooseItem(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::ChooseItem)
{
    ui->setupUi(this);
    setObjectName("ChooseItem");
    initUI();
    connect(ui->btn_delete, &QToolButton::clicked,
            [this] {
                emit sigDelete(m_layoutName);
            });
}

ChooseItem::~ChooseItem()
{
    delete ui;
}

void ChooseItem::setSelected(bool isSelected)
{
    ui->label_selected->setVisible(isSelected);
}

void ChooseItem::setEditMode(bool isEditMode)
{
    ui->btn_delete->setVisible(isEditMode);
}

QString ChooseItem::getLayoutName()
{
    return m_layoutName;
}

void ChooseItem::setNames(QString countryName, QString layoutName)
{
    ui->label_country_name->setText(countryName);
    m_layoutName = layoutName;
}

void ChooseItem::initUI()
{
    ui->btn_delete->setIcon(QIcon(":/images/delete.svg"));
    ui->btn_delete->hide();
    ui->label_selected->hide();
}

void ChooseItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void ChooseItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void ChooseItem::seletedLayoutChanged(QString selectLayout)
{
    if (m_layoutName == selectLayout)
        setSelected(true);
    else
        setSelected(false);
}
