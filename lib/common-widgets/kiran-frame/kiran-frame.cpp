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

#include "kiran-frame.h"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

using namespace Kiran::Theme;

KiranFrame::KiranFrame(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
}

void KiranFrame::setRadius(int radius)
{
    if (radius < 0 || radius == m_radius)
    {
        return;
    }
    m_radius = radius;
    update();
}

int KiranFrame::getRadius()
{
    return m_radius;
}

bool KiranFrame::getDrawBackground()
{
    return m_drawBackground;
}

void KiranFrame::setDrawBackground(bool enable)
{
    if (m_drawBackground == enable)
        return;
    m_drawBackground = enable;
    update();
}

void KiranFrame::setFixedBackgroundState(Palette::ColorGroup state)
{
    if (m_fixedBackground && m_fixedBackgroundState == state)
        return;
    m_fixedBackground = true;
    m_fixedBackgroundState = state;
    update();
}

void KiranFrame::unsetFixedBackgroundState()
{
    if (!m_fixedBackground)
        return;
    m_fixedBackground = false;
    update();
}

bool KiranFrame::getDrawBroder()
{
    return m_drawBorder;
}

void KiranFrame::setDrawBroder(bool enable)
{
    if (m_drawBorder == enable)
        return;
    m_drawBorder = enable;
    update();
}

void KiranFrame::setBorderWidth(int width)
{
    if (width <= 0)
    {
        return;
    }

    m_borderWidth = width;
    update();
}

void KiranFrame::setFixedBorderState(Palette::ColorGroup state)
{
    if (m_fixedBorder && m_fixedBorderState == state)
        return;

    m_fixedBorder = true;
    m_fixedBorderState = state;
    update();
}

void KiranFrame::unsetFixedBorderState()
{
    if (!m_fixedBorder)
        return;
    m_fixedBorder = false;
    update();
}

void KiranFrame::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    QStyle::State state;

    opt.initFrom(this);
    state = opt.state;

    QPainterPath painterPath;
    QRectF frect = opt.rect;
    frect.adjust(0.5, 0.5, -0.5, -0.5);
    painterPath.addRoundedRect(frect, m_radius, m_radius);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto kiranPalette = DEFAULT_PALETTE();
    if (m_drawBackground)
    {
        QColor backgroundColor;
        backgroundColor = m_fixedBackground ? kiranPalette->getColor(m_fixedBackgroundState, Palette::ColorRole::WIDGET) : kiranPalette->getColor(state, Palette::ColorRole::WIDGET);
        painter.fillPath(painterPath, backgroundColor);
    }

    if (m_drawBorder)
    {
        QColor borderColor;
        borderColor = m_fixedBorder ? kiranPalette->getColor(m_fixedBorderState, Palette::ColorRole::BORDER) : kiranPalette->getColor(state, Palette::ColorRole::BORDER);
        auto pen = painter.pen();
        pen.setWidth(m_borderWidth);
        pen.setColor(borderColor);
        painter.strokePath(painterPath, pen);
    }

    QWidget::paintEvent(event);
}
