/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#include "utils.h"
#include <style-palette.h>
#include <style-property.h>
#include <QIcon>
QPixmap NetworkUtils::trayIconColorSwitch(const QString &iconPath)
{
    // icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(16, 16);
    if (Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}
