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
#include "load-animation-widget.h"
#include <kiran-log/qt5-log-i.h>
#include <QPaintEvent>
#include <QPainter>
#include <iostream>

using namespace std;

#define FRAMES_PER_SECOND 40 /* frames per second */
#define FRAMES_TOTAL 78      /* total frames count */

LoadAnimationWidget::LoadAnimationWidget(QWidget *parent) : QWidget(parent),
                                                            index(0)
{
    timer.setInterval(1000 / FRAMES_PER_SECOND);
    timer.setSingleShot(false);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(renderNextFrame()));
}

LoadAnimationWidget::~LoadAnimationWidget()
{
}

void LoadAnimationWidget::resetAnimation()
{
    stopAnimation();
    index = 0;
    timer.start();
}

void LoadAnimationWidget::stopAnimation()
{
    timer.stop();
}

void LoadAnimationWidget::resumeAnimation()
{
    timer.start();
}

void LoadAnimationWidget::paintEvent(QPaintEvent *event)
{
    QString pixmap_name;
    QPixmap pixmap;
    QPainter painter(this);
    QRect drawing_rect;

    QWidget::paintEvent(event);
    pixmap_name = QString(":/icons/loading/loading-%1.png").arg(index, 2, 10, QChar('0'));

    if (!pixmap.load(pixmap_name))
    {
        KLOG_INFO() << "Failed to load pixmap " << pixmap_name;
        return;
    }

    if (pixmap.width() > width() || pixmap.height() > height())
    {
        //need to scale
        double x_scale, y_scale, scale;

        x_scale = width() * 1.0 / pixmap.width();
        y_scale = height() * 1.0 / pixmap.height();
        scale = x_scale > y_scale ? y_scale : x_scale;

        pixmap = pixmap.scaled(static_cast<int>(pixmap.width() * scale),
                               static_cast<int>(pixmap.height() * scale),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
    }

    /**
     * draw pixmap at center position
     */
    drawing_rect.setX((width() - pixmap.width()) / 2);
    drawing_rect.setY((height() - pixmap.height()) / 2);
    drawing_rect.setWidth(pixmap.width());
    drawing_rect.setHeight(pixmap.height());

    painter.drawPixmap(drawing_rect, pixmap);
}

QSize LoadAnimationWidget::sizeHint() const
{
    return QSize(80, 80);
}

void LoadAnimationWidget::renderNextFrame()
{
    index++;
    if (index > FRAMES_TOTAL)
        index = 0;
    //redraw
    repaint();
}
