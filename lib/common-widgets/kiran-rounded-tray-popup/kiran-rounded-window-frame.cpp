/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
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

#include <style-palette.h>
#include <QPainter>
#include <QPainterPath>
#include <QBitmap>

#include "kiran-rounded-window-frame.h"

using namespace Kiran;

const int KiranRoundedWindowFrame::radius = 6;

KiranRoundedWindowFrame::KiranRoundedWindowFrame(QWidget *parent)
    : QFrame(parent)
{

}

KiranRoundedWindowFrame::~KiranRoundedWindowFrame()
{
}

void KiranRoundedWindowFrame::paintEvent(QPaintEvent *event)
{
    auto stylePalette = StylePalette::instance();
    auto backgroundColor = stylePalette->color(StylePalette::Normal,StylePalette::Window,StylePalette::Background);
    auto borderColor = stylePalette->color(StylePalette::Normal,StylePalette::Window,StylePalette::Border);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath painterPath;
    QRectF rectf = rect();
    painterPath.addRoundedRect(rectf.adjusted(0.5,0.5,-0.5,-0.5),radius,radius);
    
    QPen pen;
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(borderColor);

    p.setPen(pen);
    p.setBrush(backgroundColor);
    p.drawPath(painterPath);
}