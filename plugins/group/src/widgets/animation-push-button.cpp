 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
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
 
#include <QPainter>
#include <QPropertyAnimation>
#include <QStyleOption>
#include <qt5-log-i.h>

#include "animation-push-button.h"

AnimationPushButton::AnimationPushButton(QWidget *parent)
    : QPushButton(parent),
      m_svgRender(QString(":/kcp-group-images/loading.svg"), this)
{
    initTimeLine();
}

void AnimationPushButton::setBusy(bool busy)
{
    if (m_isBusy == busy)
        return;

    if (busy && !m_svgRender.isValid())
    {
        KLOG_WARNING() << "AnimationPushButton: animation pixmap isNull!";
        return;
    }
    m_isBusy = busy;
    if (m_isBusy)
    {
        m_rotationAngle = 0;
        m_timeLine.setCurrentTime(0);
        m_timeLine.start();
    }
    else
    {
        m_timeLine.stop();
        m_timeLine.setCurrentTime(0);
        m_rotationAngle = 0;
    }
}

bool AnimationPushButton::busy()
{
    return m_isBusy;
}

void AnimationPushButton::paintEvent(QPaintEvent *event)
{
    if (m_isBusy && isEnabled())
    {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

        QStyleOption opt;
        opt.init(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

        if (m_svgRender.isValid())
        {
            painter.translate(this->rect().center());
            painter.rotate(m_rotationAngle);
            int svgDrawSize = qMin(width(), height()) - 20;
            QRect renderRect((width() - svgDrawSize) / 2 - width() / 2,
                             (height() - svgDrawSize) / 2 - height() / 2,
                             svgDrawSize,
                             svgDrawSize);
            m_svgRender.render(&painter, renderRect);
        }
    }
    else
    {
        QPushButton::paintEvent(event);
    }
}

void AnimationPushButton::initTimeLine()
{
    m_timeLine.setCurrentTime(0);
    m_timeLine.setLoopCount(0);
    m_timeLine.setUpdateInterval(50);
    m_timeLine.setDuration(750);
    m_timeLine.setFrameRange(0, 360);
    m_timeLine.setCurveShape(QTimeLine::LinearCurve);
    connect(&m_timeLine, &QTimeLine::frameChanged, [this](int value) {
        m_rotationAngle = value;
        update();
    });
}
