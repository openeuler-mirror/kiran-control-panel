/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-audio is licensed under Mulan PSL v2.
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

#include "volume-scale.h"

#include <QPainter>
#include <qt5-log-i.h>

VolumeScale::VolumeScale(QWidget *parent ) : QWidget(parent)
{

}
VolumeScale::~VolumeScale()
{

}
void VolumeScale::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawTicks(&painter);
    painter.end();
}

#define TICK_WIDTH 1
#define TICK_HEIGHT 10
#define TICK_SPACE 6
void VolumeScale::drawTicks(QPainter *painter)
{
    painter->setPen(Qt::transparent);
    int posX = 0;
    int posY = 0;
    int length = geometry().width();

    int ticksCount = length / TICK_SPACE;

    int percentTicks = m_percent * ticksCount;

    for (int i = 0; i < ticksCount; i++) {
        if(i < percentTicks)
        {
            painter->setBrush(QColor("#2eb3ff"));
        }
        else
        {
            painter->setBrush(QColor("#595959"));
        }
        painter->drawRect(posX,10,TICK_WIDTH,TICK_HEIGHT);
        posX += TICK_SPACE;
    }
}
void VolumeScale::setPercent(qreal percent)
{
    m_percent = percent;
    update();
    KLOG_DEBUG() << "m_percent:" << m_percent;
}
