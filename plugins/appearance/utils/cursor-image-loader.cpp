/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#include "cursor-image-loader.h"
#include <X11/Xcursor/Xcursor.h>

static QImage convertToNomalImage(const QImage& cursorImage)
{
    QRect rect(cursorImage.rect().bottomRight(), cursorImage.rect().topLeft());
    const quint32* pixelData = reinterpret_cast<const quint32*>(cursorImage.bits());
    for (int i = 0; i < cursorImage.height(); i++)
    {
        for (int j = 0; j < cursorImage.width(); j++)
        {
            if (*pixelData)
            {
                if (i < rect.left()) rect.setLeft(i);
                if (i > rect.right()) rect.setRight(i);
                if (j < rect.top()) rect.setTop(j);
                if (j > rect.bottom()) rect.setBottom(j);
                pixelData++;
            }
        }
    }
    return cursorImage.copy(rect.normalized());
}

QImage CursorImageLoader::getCursorImage(const QString& cursorTheme, const QString& cursorName, int size)
{
    XcursorImage* xCursorImage = XcursorLibraryLoadImage(cursorName.toStdString().c_str(),
                                                         cursorTheme.toStdString().c_str(),
                                                         size);
    if (!xCursorImage)
    {
        return QImage();
    }

    QImage img(reinterpret_cast<uchar*>(xCursorImage->pixels),
               xCursorImage->width, xCursorImage->height,
               QImage::Format_ARGB32_Premultiplied);

    QImage cursorImg = convertToNomalImage(img);
    XcursorImageDestroy(xCursorImage);

    return cursorImg;
}
