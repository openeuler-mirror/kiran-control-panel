#include "preview-label.h"
#include "../wallpaper-global.h"
#include <QVBoxLayout>
#include <QPainter>
#include <math.h>

PreviewLabel::PreviewLabel(int wallpaperType, QString imgPath, QWidget *parent):
    QLabel(parent)
{
    setFixedSize(180,90);
    setAlignment(Qt::AlignHCenter);
    m_wallpaperImg = loadPixmap(imgPath);
    m_wallpaperType = wallpaperType;
}

void PreviewLabel::setWallpaperPath(QString path)
{
    m_wallpaperImg = loadPixmap(path);
    update();
}

void PreviewLabel::setWallpaperType(int type)
{
    m_wallpaperType = type;
}

void PreviewLabel::updateWallpaper(int type, QString path)
{
    m_wallpaperImg = loadPixmap(path);
    m_wallpaperType = type;
    update();
}

void PreviewLabel::drawDesktopPreview(QPainter *painter)
{
    //load bg
    painter->save();
    if(!m_wallpaperImg.isNull())
    {
        painter->drawPixmap(this->rect(),m_wallpaperImg);
    }
    //draw rect
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0,0,0,180));

    for(int i=0; i<6; i++)
    {
        QRect rect(this->rect().x()+5,this->rect().y()+5+i*12,8,8);
        QPainterPath path;
        path.addRoundedRect(rect , 3, 3);
        painter->setClipPath(path);
        painter->drawPath(path);
    }
    for(int i=0;i<4; i++)
    {
        QRect rect(this->rect().x()+18,this->rect().y()+5+i*12,8,8);
        QPainterPath path;
        path.addRoundedRect(rect , 3, 3);
        painter->setClipPath(path);
        painter->drawPath(path);
    }

    QRect rectBottom(this->rect().x()+3,this->rect().height()-8,this->rect().width()-6,5);
    QPainterPath path2;
    path2.addRoundedRect(rectBottom , 3, 3);
    painter->setClipPath(path2);
    painter->drawPath(path2);
    painter->restore();
}

void PreviewLabel::drawLockScreenPreview(QPainter *painter)
{
    int width = this->rect().width();
    int height = this->rect().height();
    int pieRadius = 8;
    int centerRectHeight = 5;
    int centerRectWidth = 30;
    int bottomRect = 8;

    painter->save();
    if(!m_wallpaperImg.isNull())
    {
        painter->drawPixmap(this->rect(),m_wallpaperImg);
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0,0,0,180));

    //drawpie
    painter->drawEllipse(QPoint(width/2.0,height/2.0-pieRadius),pieRadius,pieRadius);

    //draw center rect
    QRect rectCenter(width/2.0-centerRectWidth/2.0,height/2.0+8, centerRectWidth,centerRectHeight);
    QPainterPath path;
    path.addRoundedRect(rectCenter , 3, 3);
    painter->setClipPath(path);
    painter->drawPath(path);

    //draw bottom rect
    for(int i=0; i<=3; i++)
    {
        QRect rect(width-15-10*i,height-15,bottomRect,bottomRect);
        QPainterPath path;
        path.addRoundedRect(rect , 3, 3);
        painter->setClipPath(path);
        painter->drawPath(path);
    }

    painter->restore();
}

QPixmap PreviewLabel::loadPixmap(QString imagePath)
{
    QPixmap pixmap;
    QSize size(180,90);
    pixmap.load(imagePath);

    QSize pixmapSize = pixmap.size();
    qreal scaleFactor = qMax(size.width() / (double) pixmapSize.width(), size.height() / (double) pixmapSize.height());

    QSize generatePixmapSize = QSize(floor(pixmapSize.width() * scaleFactor + 0.5),
                                     floor(pixmapSize.height() * scaleFactor + 0.5));

    QPixmap scaledPixmap = pixmap.scaled(generatePixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return scaledPixmap;
}

void PreviewLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    if(m_wallpaperType == DESKTOP)
    {
        drawDesktopPreview(&painter);
    }
    else
    {
        drawLockScreenPreview(&painter);
    }
    QLabel::paintEvent(event);
}
