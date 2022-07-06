//
// Created by liuxinhao on 2022/5/27.
//

#include "category-item.h"

#include <QAbstractButton>
#include <QPainter>
#include <QPainterPath>
#include <style-palette.h>

using namespace Kiran;

CategoryItem::CategoryItem(QWidget *parent) : QAbstractButton(parent)
{
    setCheckable(true);
}

CategoryItem::~CategoryItem()
{
}

void CategoryItem::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor background;
    auto kiranPalette = StylePalette::instance();

    StylePalette::ColorState colorState = StylePalette::Normal;
    if( isChecked() )
        colorState = StylePalette::Checked;
    else if( underMouse() )
        colorState = StylePalette::Hover;

    if( parentWidget() && colorState!=StylePalette::Normal )
    {
        QPainterPath path;
        path.addRoundedRect(rect(), 6, 6);
        background = kiranPalette->color(colorState,StylePalette::Widget,StylePalette::Background);
        p.fillPath(path, background);
    }

    QSize iconSize(40, 40);
    QRect iconRect(QPoint(12, (this->height() - iconSize.height()) / 2), iconSize);

    QPixmap pixmap = icon().pixmap(iconSize);
    p.drawPixmap(iconRect, pixmap);

    QRect textRect(iconRect.right() + 13, rect().top(), rect().right() - iconRect.right() - 10, rect().height());
    if( isChecked() )
    {
        p.setPen(QColor("white"));
    }
    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
}
