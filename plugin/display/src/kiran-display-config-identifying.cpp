/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-display is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 */

#include "kiran-display-config-identifying.h"
#include <QEventLoop>
#include <QPainter>
#include <QEvent>
#include <QTimer>

KiranDisplayConfigIdentifying::KiranDisplayConfigIdentifying(QWidget *parent) :
    QWidget(parent)
{
    m_loop = new QEventLoop(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog/*|Qt::WindowStaysOnTopHint|Qt::Popup*/);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setMouseTracking(true);
    setFixedSize(400, 300);
    installEventFilter(this);

    QTimer::singleShot(3000, this, [=](){this->hide();});
}

KiranDisplayConfigIdentifying::~KiranDisplayConfigIdentifying()
{
}

void KiranDisplayConfigIdentifying::exec(const QPoint &pos)
{
    move(pos+QPoint(6, 6));
    show();

    m_loop->exec();
}

void KiranDisplayConfigIdentifying::hideEvent(QHideEvent *event)
{
    m_loop->quit();
    QWidget::hideEvent(event);
}

void KiranDisplayConfigIdentifying::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), QBrush(QColor(34, 34, 34)));
    QPen pen;
    pen.setWidth(2);
    pen.setColor("#4b4b4b");
    painter.setPen(pen);
    QRect rect = this->rect();
    rect.adjust(0, 0, -1, -1);
    painter.drawRect(rect);

    QFont font;
    font.setPixelSize(62);
    painter.setFont(font);

    pen.setWidth(1);
    pen.setColor("#ffffff");
    painter.setPen(pen);
    painter.drawText(this->rect(), Qt::TextWrapAnywhere|Qt::AlignCenter, m_text);
}

bool KiranDisplayConfigIdentifying::eventFilter(QObject *object, QEvent *event)
{
    if(object == this)
    {
        if(event->type() == QEvent::WindowDeactivate)
        {
            hide();
        }
    }

    return QWidget::eventFilter(object, event);
}

void KiranDisplayConfigIdentifying::setText(const QString &text)
{
    m_text = text;
}

