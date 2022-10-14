/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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
#include <QIcon>
#include <qt5-log-i.h>
using namespace NetworkManager;

QPixmap NetworkUtils::trayIconColorSwitch(const QString &iconPath, const int iconSize)
{
    // icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(iconSize, iconSize);
    if (Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}

QPixmap NetworkUtils::trayIconColorSwitch(QPixmap pixmap)
{
    if (!pixmap.isNull())
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        return QPixmap::fromImage(image);
    }
    return QPixmap();
}


NetworkManager::Device::List NetworkUtils::getDeviceList(NetworkManager::Device::Type type)
{
    const Device::List deviceList = networkInterfaces();
    Device::List list;
    for (Device::Ptr dev : deviceList)
    {
        if(dev->type() == type)
        {
            list << dev;
        }
    }
    return list;
}


Device::List NetworkUtils::getAvailableDeviceList(NetworkManager::Device::Type type)
{
    const Device::List deviceList = networkInterfaces();
    Device::List list;
    for (Device::Ptr dev : deviceList)
    {
        if(dev->type() == type)
        {
            if (dev->state() == Device::Unmanaged)
                continue;
            if (dev->state() == Device::Unavailable)
                continue;
            list << dev;
        }
    }
    return list;
}
