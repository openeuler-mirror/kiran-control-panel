//
// Created by liuxinhao on 2022/6/17.
//

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
