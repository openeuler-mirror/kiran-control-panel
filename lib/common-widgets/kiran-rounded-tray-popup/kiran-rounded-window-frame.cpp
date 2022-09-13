//
// Created by liuxinhao on 2022/6/24.
//

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