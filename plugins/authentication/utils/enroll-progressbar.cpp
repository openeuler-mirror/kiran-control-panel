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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "enroll-progressbar.h"
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>

/* 进度条线段长度 */
const unsigned int EnrollProgressBar::m_paintProgressLength = 18;
/* 进度条线段数量 */
const unsigned int EnrollProgressBar::m_progressLinesCount = 80;
/* 进度条线段距边框距离 */
const unsigned int EnrollProgressBar::m_borerMargin = 4;

EnrollProgressBar::EnrollProgressBar(QWidget *parent) : QWidget(parent)
{
    m_animation = new QPropertyAnimation(this);
    m_animation->setTargetObject(this);
    m_animation->setPropertyName("progressValue");
    m_animation->setDuration(500);
    m_animation->setEasingCurve(QEasingCurve::OutQuad);
}

EnrollProgressBar::~EnrollProgressBar()
{
}

void EnrollProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    painter.save();
    paintProgressBar(painter);
    painter.restore();
}

void EnrollProgressBar::resizeEvent(QResizeEvent *event)
{
    adjustCenterWidget(event->size());
    QWidget::resizeEvent(event);
}

void EnrollProgressBar::setProgressValue(unsigned int value)
{
    m_progressValue = value;
    update();
}

void EnrollProgressBar::setCenterWidget(QWidget *widget)
{
    m_centerWidget = widget;
    adjustCenterWidget(size());
}

void EnrollProgressBar::setProgress(uint progress)
{
    if (m_animationEnable)
    {
        m_animation->stop();
        m_animation->setStartValue(m_progressValue);
        m_animation->setEndValue(progress);
        m_animation->start();
    }
    else
    {
        setProgressValue(progress);
    }
}

void EnrollProgressBar::setAnimationEnable(bool enable)
{
    m_animationEnable = enable;
}

void EnrollProgressBar::adjustCenterWidget(QSize size)
{
    if( !m_centerWidget )
    {
        return;
    }

    QSize centerWidgetSize(size.height() - (2 * (m_borerMargin + m_paintProgressLength)),
                           size.height() - (2 * (m_borerMargin + m_paintProgressLength)));
    if( centerWidgetSize.width() < 0 || centerWidgetSize.height() < 0 )
    {
        return;
    }

    m_centerWidget->setFixedSize(centerWidgetSize);
    m_centerWidget->setParent(this);

    QRect centerWidgetRect(QPoint(0, 0), centerWidgetSize);
    centerWidgetRect.moveCenter(rect().center());
    m_centerWidget->setGeometry(centerWidgetRect);

    QBitmap bitMap(centerWidgetSize);
    bitMap.fill();

    QPainter painter(&bitMap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath painterPath;
    painterPath.addRoundedRect(QRectF(QPoint(0, 0), centerWidgetSize), centerWidgetSize.height() / 2, centerWidgetSize.height() / 2);
    painter.fillPath(painterPath, Qt::black);
    painter.end();

    m_centerWidget->setMask(bitMap);
}

void EnrollProgressBar::paintProgressBar(QPainter &painter)
{
    QColor normalColor("#393939");
    QColor activeColor("#2eb3ff");

    painter.translate(width() / 2, height() / 2);
    QPen pen;
    pen.setWidthF(1.4);
    qreal rotateAngle = 0;
    for (unsigned int i = 0; i < m_progressLinesCount; i++)
    {
        qreal percent = rotateAngle / 360;
        unsigned int iPercent = percent * 100;
        if (iPercent < m_progressValue)
        {
            pen.setColor(activeColor);
        }
        else
        {
            pen.setColor(normalColor);
        }
        painter.setPen(pen);

        painter.drawLine(0, -(height() / 2) + m_borerMargin, 0, -(height() / 2) + m_paintProgressLength);
        painter.rotate(360.0 / m_progressLinesCount);
        rotateAngle += 360.0 / m_progressLinesCount;
    }
}
