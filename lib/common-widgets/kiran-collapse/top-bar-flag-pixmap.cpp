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
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */

#include "top-bar-flag-pixmap.h"
#include <QIcon>

QPixmap FlagPixmap::expansionFlagPixmap()
{
    // TODO: 后续使用KiranIcon代替QLable，无需跟随主题变化转换像素
    auto icon = QIcon::fromTheme(RIGHT_ARROW);
    auto pixmap = icon.pixmap(QSize(16, 16));
    auto image = pixmap.toImage();

    QImage transformedImage = rotateImage(image, 90.0);
    return QPixmap::fromImage(transformedImage);
}

QPixmap FlagPixmap::collapseFlagPixmap()
{
    // TODO: 后续使用KiranIcon代替QLable，无需跟随主题变化转换像素
    auto icon = QIcon::fromTheme(RIGHT_ARROW);
    auto pixmap = icon.pixmap(QSize(16, 16));
    auto image = pixmap.toImage();

    QImage transformedImage = rotateImage(image, -90.0);
    return QPixmap::fromImage(transformedImage);
}

QPixmap FlagPixmap::expansionFlagPixmapDark()
{
    // TODO: 后续使用KiranIcon代替QLable，无需跟随主题变化转换像素
    auto icon = QIcon::fromTheme(RIGHT_ARROW);
    auto pixmap = icon.pixmap(QSize(16, 16));
    auto image = pixmap.toImage();

    QImage transformedImage = rotateImage(image, 90.0);
    invertColors(transformedImage);
    return QPixmap::fromImage(transformedImage);
}

QPixmap FlagPixmap::collapseFlagPixmapDark()
{
    // TODO: 后续使用KiranIcon代替QLable，无需跟随主题变化转换像素
    auto icon = QIcon::fromTheme(RIGHT_ARROW);
    auto pixmap = icon.pixmap(QSize(16, 16));
    auto image = pixmap.toImage();

    QImage transformedImage = rotateImage(image, -90.0);
    invertColors(transformedImage);
    return QPixmap::fromImage(transformedImage);
}

QImage FlagPixmap::rotateImage(const QImage& image, qreal angle)
{
    QTransform transform;
    transform.rotate(angle);
    return image.transformed(transform, Qt::FastTransformation);
}

void FlagPixmap::invertColors(QImage& image)
{
    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            QColor color = image.pixelColor(x, y);
            QColor invertedColor = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());
            image.setPixelColor(x, y, invertedColor);
        }
    }
}