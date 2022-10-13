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

void CategoryItem::setCategoryID(const QString &id)
{
    m_categoryID = id;
}

QString CategoryItem::getCategoryID()
{
    return m_categoryID;
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
