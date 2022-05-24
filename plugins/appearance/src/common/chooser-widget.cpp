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
#include "include.h"
#include "ui_chooser-widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <kiran-palette.h>
#include <QPainterPath>

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

void ChooserWidget::setName(QString name)
{
    ui->label_name->setText(name);
}

void ChooserWidget::initUI(QString title)
{
    ui->label_text->setText(title);
    ui->label_arrow->setFixedSize(16, 16);
    ui->label_arrow->setPixmap(QPixmap(":/kcp-appearance/images/select.svg"));
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

    KiranPalette::ColorState colorState = KiranPalette::Normal;
    if( !(opt.state & QStyle::State_Enabled) )
    {
        colorState = KiranPalette::Disabled;
    }
    else if( opt.state & QStyle::State_Sunken )
    {
        colorState = KiranPalette::Active;
    }
    else if ( opt.state & QStyle::State_MouseOver )
    {
        colorState = KiranPalette::Hover;
    }

    auto kiranPalette = KiranPalette::instance();
    auto background = kiranPalette->color(colorState,KiranPalette::Widget,KiranPalette::Background);

    QPainter p(this);
    QPainterPath painterPath;
    painterPath.addRoundedRect(opt.rect,6,6);
    p.fillPath(painterPath,background);
}
