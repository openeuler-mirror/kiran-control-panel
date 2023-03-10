#include "finger-enroll-progressbar.h"
#include "pixmap-preview.h"
#include <QLabel>
#include <QTimer>
#include <QDebug>

const std::list<std::tuple<uint, QString>> FingerEnrollProgressBar::m_progressRangePixmapList = {
    {0, ":/kcp-authentication/images/finger-0.svg"},
    {25, ":/kcp-authentication/images/finger-25.svg"},
    {50, ":/kcp-authentication/images/finger-50.svg"},
    {75, ":/kcp-authentication/images/finger-75.svg"},
    {100, ":/kcp-authentication/images/finger-100.svg"}};

FingerEnrollProgressBar::FingerEnrollProgressBar(QWidget* parent)
    :EnrollProgressBar(parent)
{
    init();
}

FingerEnrollProgressBar::~FingerEnrollProgressBar()
{
    
}
void FingerEnrollProgressBar::setProgress(uint progress)
{
    QString progressPixmap;
    for(auto iter:m_progressRangePixmapList)
    {
        auto min = std::get<0>(iter);
        if( progress >= min )
        {
            progressPixmap = std::get<1>(iter);
        }
    }
    m_fingerWidget->setPixmap(progressPixmap);
    EnrollProgressBar::setProgress(progress);
}

void FingerEnrollProgressBar::init()
{
    m_fingerWidget = new PixmapPreview();
    setCenterWidget(m_fingerWidget);
    setProgress(0);
}
