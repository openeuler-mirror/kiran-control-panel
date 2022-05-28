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
#include "kiran-scroll-area.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>

KiranScrollArea::KiranScrollArea(QWidget* parent)
    : QScrollArea(parent)
{
}

KiranScrollArea::~KiranScrollArea()
{
}

QSize KiranScrollArea::sizeHint() const
{
    if (!widget())
    {
        return QScrollArea::sizeHint();
    }

    QSize areaSizeHint = QSize(636, 690);
    areaSizeHint += QSize(verticalScrollBar()->width(), horizontalScrollBar()->height());

    return areaSizeHint;
}

void KiranScrollArea::paintEvent(QPaintEvent* painEvent)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this->viewport());
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(painEvent);
}
