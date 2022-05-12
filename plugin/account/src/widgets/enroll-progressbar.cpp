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
 

#include "enroll-progressbar.h"
#include <QPainter>

EnrollProgressBar::EnrollProgressBar(QWidget *parent) : QWidget(parent)
{
    startTimer(100);
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

    paintCenterPixmap(painter, m_centerPixmap);
}

void EnrollProgressBar::setProgressValue(unsigned int value)
{
    m_progressValue = value;
    update();
}

void EnrollProgressBar::paintProgressBar(QPainter &painter)
{
    QColor normalColor("#2d2d2d");
    QColor activeColor("#2eb3ff");
    painter.translate(width() / 2, height() / 2);
    QPen pen;
    pen.setWidthF(1.4);
    qreal rotateAngle = 0;
    for (int i = 0; i < m_progressLinesCount; i++)
    {
        qreal percent = rotateAngle / 360;
        int iPercent = percent * 100;
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

void EnrollProgressBar::paintCenterPixmap(QPainter &painter, const QPixmap &pixmap)
{
    QPoint center = this->rect().center();
    qreal imageRadius = getCenterImageRadius();
    QPainterPath painterPath;
    painterPath.addEllipse(center.x() - imageRadius, center.y() - imageRadius, imageRadius * 2, imageRadius * 2);
    painter.setClipPath(painterPath);
    painter.drawPixmap(center.x() - (m_centerPixmap.size().width() / 2),
                       center.y() - (m_centerPixmap.size().height() / 2),
                       m_centerPixmap.size().width(),
                       m_centerPixmap.size().height(),
                       m_centerPixmap);
}

qreal EnrollProgressBar::getCenterImageRadius()
{
    qreal imageRadius = (height() / 2.0 - m_borerMargin - m_paintProgressLength);
    return imageRadius;
}

void EnrollProgressBar::updateCenterImage(const QPixmap &centerPixmap)
{
    m_centerPixmap = centerPixmap;
    update();
}
