/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#include "animation-loading-label.h"
#include <QPainter>

AnimationLoadingLabel::AnimationLoadingLabel(QWidget *parent)
    : QLabel(parent),
      m_dashOffset(0),
      m_dashLength(89),
      m_angle(0),
      m_isLoading(false)
{
    setSize(16);
    init();
}

void AnimationLoadingLabel::init()
{
    this->setFixedSize(32,32);

    m_group = new QParallelAnimationGroup(this);
    m_group->setLoopCount(-1);

    m_animation = new QPropertyAnimation(this);
    m_animation->setPropertyName("dashLength");
    m_animation->setTargetObject(this);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
    m_animation->setStartValue(0.1);
    m_animation->setKeyValueAt(0.15, 0.2);
    m_animation->setKeyValueAt(0.6, 20);
    m_animation->setKeyValueAt(0.7, 20);
    m_animation->setEndValue(20);
    m_animation->setDuration(2050);

    m_group->addAnimation(m_animation);

    m_animation = new QPropertyAnimation(this);
    m_animation->setPropertyName("dashOffset");
    m_animation->setTargetObject(this);
    m_animation->setEasingCurve(QEasingCurve::InOutSine);
    m_animation->setStartValue(0);
    m_animation->setKeyValueAt(0.15, 0);
    m_animation->setKeyValueAt(0.6, -7);
    m_animation->setKeyValueAt(0.7, -7);
    m_animation->setEndValue(-25);
    m_animation->setDuration(2050);

    m_group->addAnimation(m_animation);

    m_animation = new QPropertyAnimation(this);
    m_animation->setPropertyName("angle");
    m_animation->setTargetObject(this);
    m_animation->setStartValue(0);
    m_animation->setEndValue(719);
    m_animation->setDuration(2050);

    m_group->addAnimation(m_animation);
}

void AnimationLoadingLabel::paintEvent(QPaintEvent *event)
{
    if (m_isLoading)
    {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.translate(width() / 2, height() / 2);
        painter.rotate(m_angle);

        QPen pen;
        pen.setCapStyle(Qt::RoundCap);
        pen.setWidthF(6);
        pen.setColor(QColor("#393939"));

        QVector<qreal> pattern;
        pattern << m_dashLength * m_size / 50 << 30 * m_size / 50;

        pen.setDashOffset(m_dashOffset * m_size / 50);
        pen.setDashPattern(pattern);

        painter.setPen(pen);

        painter.drawEllipse(QPoint(0, 0), m_size / 2, m_size / 2);
    }
    else
        QLabel::paintEvent(event);
}

void AnimationLoadingLabel::setSize(int size)
{
    m_size = size;
}

void AnimationLoadingLabel::setLoadingStatus(bool isLoading)
{
    m_isLoading = isLoading;
    if(isLoading)
        m_group->start();
    else
        m_group->stop();
}
