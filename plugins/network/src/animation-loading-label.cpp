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
#include <QPainterPath>
AnimationLoadingLabel::AnimationLoadingLabel(QWidget *parent)
    : QLabel(parent),
      m_angle(0),
      m_isLoading(false)
{
    setRadiusSize(8);
    setLineWidth(2);
    init();
}

void AnimationLoadingLabel::init()
{
    this->setFixedSize(16, 16);
    m_group = new QParallelAnimationGroup(this);
    m_group->setLoopCount(-1);

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

        QConicalGradient gra(QPoint(0, 0), 0);
        gra.setColorAt(0, QColor("#3BB6FE"));
        gra.setColorAt(1, QColor("#FFFFFF"));
        QBrush brush(gra);

        QRect rect(-m_radiusSize, -m_radiusSize, m_radiusSize * 2, m_radiusSize * 2);
        QPainterPath path;
        path.arcTo(rect, 0, 270);

        QPainterPath subPath;
        subPath.addEllipse(rect.adjusted(m_lineWidth, m_lineWidth, -m_lineWidth, -m_lineWidth));

        path = path - subPath;
        painter.setBrush(brush);
        painter.setPen(Qt::NoPen);
        painter.drawPath(path);
    }
    else
        QLabel::paintEvent(event);
}

void AnimationLoadingLabel::setRadiusSize(int size)
{
    m_radiusSize = size;
}
void AnimationLoadingLabel::setLineWidth(int size)
{
    m_lineWidth = size;
}

void AnimationLoadingLabel::setLoadingStatus(bool isLoading)
{
    m_isLoading = isLoading;
    if (isLoading)
        m_group->start();
    else
        m_group->stop();
}
