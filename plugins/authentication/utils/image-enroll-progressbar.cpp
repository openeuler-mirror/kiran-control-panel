/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
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
#include "image-enroll-progressbar.h"
#include <qt5-log-i.h>
#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QTimer>

#include "pixmap-preview.h"

#define IMG_SIZE QSize()
ImageEnrollProgressBar::ImageEnrollProgressBar(QWidget* parent)
    : EnrollProgressBar(parent)
{
    init();
}

ImageEnrollProgressBar::~ImageEnrollProgressBar()
{
}

void ImageEnrollProgressBar::registerPercentImage(uint percent, const QString& img)
{
    auto iter = m_progressRangePixmapList.begin();
    for (; iter != m_progressRangePixmapList.end(); iter++)
    {
        auto iterPercent = std::get<0>(*iter);
        auto iterImg = std::get<1>(*iter);

        if (percent >= iterPercent)
        {
            break;
        }
    }

    if (iter == m_progressRangePixmapList.end() && !m_progressRangePixmapList.empty())
    {
        KLOG_WARNING() << "can't register percent image:" << percent << img;
        return;
    }

    m_progressRangePixmapList.insert(iter, std::make_tuple(percent, img));
}

void ImageEnrollProgressBar::registerPercentImages(const std::list<std::tuple<uint, QString>>& percentImages)
{
    m_progressRangePixmapList = percentImages;
}

void ImageEnrollProgressBar::setProgress(uint progress)
{
    QPixmap pixmap;
    for (auto iter : m_progressRangePixmapList)
    {
        auto min = std::get<0>(iter);
        if (progress >= min)
        {
            QString imgPath = std::get<1>(iter);
            // TODO: 后续使用KiranIcon代替QLable，无需跟随主题变化转换像素
            QIcon icon(QIcon::fromTheme(imgPath));
            pixmap = icon.pixmap(m_fingerWidget->size());
        }
    }

    m_fingerWidget->setPixmap(pixmap);
    EnrollProgressBar::setProgress(progress);
}

void ImageEnrollProgressBar::init()
{
    m_fingerWidget = new PixmapPreview();
    setCenterWidget(m_fingerWidget);
}
