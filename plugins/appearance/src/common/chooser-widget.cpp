/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
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

#include "chooser-widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "include.h"
#include "ui_chooser-widget.h"

ChooserWidget::ChooserWidget(QString title, int type, QWidget *parent) : QWidget(parent),
                                                                         ui(new Ui::ChooserWidget)
{
    ui->setupUi(this);
    initUI(title);
    m_wallpaperType = type;
}

ChooserWidget::~ChooserWidget()
{
    delete ui;
}

/**
 * @brief ChooserWidget::setName 设置选择控件名字
 * @param name 名字
 */
void ChooserWidget::setName(QString name)
{
    ui->label_name->setText(name);
}

void ChooserWidget::initUI(QString title)
{
    ui->label_text->setText(title);
    ui->label_arrow->setFixedSize(16, 16);
    ui->label_arrow->setPixmap(QPixmap(":/images/select.svg"));
    ui->label_name->setStyleSheet("#label_name{color: #919191;}");
    setStyleSheet("#ChooserWidget{background-color: #2d2d2d;border-radius : 6px;}");
}

void ChooserWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void ChooserWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
