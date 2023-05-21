#include "image-enroll-progressbar.h"
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <qt5-log-i.h>
#include <QIcon>

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
    for ( ; iter != m_progressRangePixmapList.end(); iter++)
    {
        auto iterPercent = std::get<0>(*iter);
        auto iterImg = std::get<1>(*iter);

        if( percent >= iterPercent )
        {
            break;
        }
    }

    if( iter == m_progressRangePixmapList.end() && !m_progressRangePixmapList.empty() )
    {
        KLOG_WARNING() << "can't register percent image:" << percent << img;
        return;
    }

    m_progressRangePixmapList.insert(iter,std::make_tuple(percent,img));
}

void ImageEnrollProgressBar::registerPercentImages(const std::list<std::tuple<uint,QString>>& percentImages)
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
            QIcon icon(imgPath);
            pixmap= icon.pixmap(m_fingerWidget->size());
        }
    }

    m_fingerWidget->setPixmap(pixmap);
    EnrollProgressBar::setProgress(progress);
}

void ImageEnrollProgressBar::init()
{
    m_fingerWidget = new PixmapPreview();
    setCenterWidget(m_fingerWidget);
    setProgress(0);
}
