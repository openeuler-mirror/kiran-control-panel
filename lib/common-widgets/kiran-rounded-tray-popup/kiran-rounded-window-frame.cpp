//
// Created by liuxinhao on 2022/6/24.
//

#include <style-palette.h>
#include <QPainter>
#include <QPainterPath>
#include "kiran-rounded-window-frame.h"

using namespace Kiran;

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
    painterPath.addRoundedRect(rect(),6,6);

    p.setPen(borderColor);
    p.setBrush(backgroundColor);
    p.drawPath(painterPath);
}
