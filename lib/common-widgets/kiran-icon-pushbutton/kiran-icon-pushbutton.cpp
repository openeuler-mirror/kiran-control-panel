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

#include "kiran-icon-pushbutton.h"

using namespace Kiran;

KiranIconPushButton::KiranIconPushButton(QWidget *parent)
    : QPushButton(parent)
{
    connect(StylePalette::instance(),&StylePalette::themeChanged,this,&KiranIconPushButton::updatePixmapForTheme);
}

KiranIconPushButton::~KiranIconPushButton()
{

}

void KiranIconPushButton::setThemeIcon(QPixmap pixmap, PaletteType paletteType)
{
    m_pixmap = pixmap;
    m_iconPaletteType = paletteType;
    updatePixmapForTheme();
}

void KiranIconPushButton::updatePixmapForTheme()
{
    if( m_pixmap.isNull() || m_iconPaletteType==PALETTE_LAST )
        return;

    auto kiranPalette = StylePalette::instance();
    QPixmap pixmap(m_pixmap);
    if( kiranPalette->paletteType() != m_iconPaletteType )
    {
        auto image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }

    setIcon(pixmap);
}
