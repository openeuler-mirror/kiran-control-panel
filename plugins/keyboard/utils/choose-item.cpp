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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "choose-item.h"
#include "ui_choose-item.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

ChooseItem::ChooseItem(QWidget *parent) : KiranFrame(parent),
                                          ui(new Ui::ChooseItem)
{
    ui->setupUi(this);
    setObjectName("ChooseItem");
    initUI();
    connect(ui->btn_delete, &QToolButton::clicked,[this] {
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
    setDrawBroder(false);
    setAttribute(Qt::WA_Hover);
    ui->btn_delete->setIcon(QIcon(":/kiran-control-panel/images/trash.svg"));
    ui->btn_delete->hide();

    QPixmap pixmap(":/kiran-control-panel/images/indicator-selected.png");
    ui->label_selected->setPixmap(pixmap);
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


void ChooseItem::seletedLayoutChanged(QString selectLayout)
{
    if (m_layoutName == selectLayout)
        setSelected(true);
    else
        setSelected(false);
}
