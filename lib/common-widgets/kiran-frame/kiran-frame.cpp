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
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QDebug>

using namespace Kiran;

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

void KiranFrame::setFixedBackgroundState(Kiran::StylePalette::ColorState state)
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
    if( width <= 0)
    {
        return;
    }

    m_borderWidth = width;
    update();
}

void KiranFrame::setFixedBorderState(Kiran::StylePalette::ColorState state)
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
    frect.adjust(0.5,0.5,-0.5,-0.5);
    painterPath.addRoundedRect(frect, m_radius, m_radius);

    auto getStateFunc = [this](QStyle::State state) -> StylePalette::ColorState
    {
        if (!(state & QStyle::State_Enabled))
        {
            return StylePalette::Disabled;
        }
        else if (state & QStyle::State_Sunken)
        {
            return StylePalette::Active;
        }
        else if  ( (state & QStyle::State_MouseOver) && testAttribute(Qt::WA_Hover) )
        {
            return StylePalette::Hover;
        }
        else
        {
            return StylePalette::Normal;
        }
    };

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto kiranPalette = StylePalette::instance();
    if (m_drawBackground)
    {
        QColor backgroundColor;
        backgroundColor = kiranPalette->color(m_fixedBackground ? m_fixedBackgroundState : getStateFunc(state),
                                              StylePalette::Widget,
                                              StylePalette::Background);
        painter.fillPath(painterPath,backgroundColor);
    }

    if(m_drawBorder)
    {
        QColor borderColor;
        borderColor = kiranPalette->color(m_fixedBorder ? m_fixedBorderState : getStateFunc(state),
                                          StylePalette::Widget,
                                          StylePalette::Border);
        auto pen = painter.pen();
        pen.setWidth(m_borderWidth);
        pen.setColor(borderColor);
        painter.strokePath(painterPath,pen);
    }

    QWidget::paintEvent(event);
}
