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

#include <palette.h>
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QX11Info>

#include "kiran-rounded-window-frame.h"

using namespace Kiran::Theme;

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
    auto palette = DEFAULT_PALETTE();
    auto backgroundColor = palette->getColor(Palette::ColorGroup::ACTIVE, Palette::ColorRole::WINDOW);
    auto borderColor = palette->getColor(Palette::ColorGroup::ACTIVE, Palette::ColorRole::BORDER);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath painterPath;
    QRectF rectf = rect();

    int tmpRadius = QX11Info::isCompositingManagerRunning() ? radius : 0;
    painterPath.addRoundedRect(rectf.adjusted(0.5, 0.5, -0.5, -0.5), tmpRadius, tmpRadius);

    QPen pen;
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(borderColor);

    p.setPen(pen);
    p.setBrush(backgroundColor);
    p.drawPath(painterPath);
}