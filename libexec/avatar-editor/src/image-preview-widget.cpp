/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
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

#include <QBitmap>
#include <QDebug>
#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QTimer>
#include <QWheelEvent>

#include "image-preview-widget.h"
#include "ui-defines.h"

ImagePreviewWidget::ImagePreviewWidget(QWidget *parent) : QWidget(parent)
{
}

ImagePreviewWidget::~ImagePreviewWidget()
{
}

void ImagePreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing);

    if (!m_scaledPixmap.isNull())
    {
        painter.drawPixmap(m_pixmapLefttop, m_scaledPixmap);
    }

    QPainterPath maskPath;
    QPoint center = rect().center();
    QRectF circleRect((width() - CUTTING_CIRCLE_RADIUS * 2) / 2.0,
                      (height() - CUTTING_CIRCLE_RADIUS * 2) / 2.0,
                      CUTTING_CIRCLE_RADIUS * 2,
                      CUTTING_CIRCLE_RADIUS * 2);
    maskPath.moveTo(0, 0);
    maskPath.lineTo(width(), 0);
    maskPath.lineTo(width(), height());
    maskPath.lineTo(0, height());
    maskPath.lineTo(0, 0);
    maskPath.moveTo(center);
    maskPath.arcTo(circleRect, 0.0, 360.0);
    maskPath.closeSubpath();
    painter.fillPath(maskPath, QColor(0, 0, 0, 255 * 0.6));

    QPen pen = painter.pen();
    pen.setColor(QColor(255, 255, 255, 255 * 0.4));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawEllipse(circleRect);
}

void ImagePreviewWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_btnIsPressed = true;
        m_btnPressedPoint = event->pos();
    }
}

void ImagePreviewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_btnIsPressed)
    {
        m_btnIsPressed = false;
        m_btnPressedPoint = QPoint(0, 0);
        if (!imagePositionVerification())
        {
            imageAutomaticPosition();
            update();
        }
    }
}

void ImagePreviewWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_btnIsPressed)
    {
        QPoint curPos = event->pos();
        int x_distance = curPos.x() - m_btnPressedPoint.x();
        int y_distance = curPos.y() - m_btnPressedPoint.y();
        m_pixmapLefttop.setX(m_pixmapLefttop.x() + x_distance);
        m_pixmapLefttop.setY(m_pixmapLefttop.y() + y_distance);
        m_btnPressedPoint = curPos;
        update();
    }
}

void ImagePreviewWidget::setSrcImagePath(const QString &imagePath)
{
    if (m_srcImagePath == imagePath)
    {
        return;
    }
    m_srcImagePath = imagePath;
    loadSrcImage();
}

QString ImagePreviewWidget::srcImagePath()
{
    return m_srcImagePath;
}

void ImagePreviewWidget::setDstImagePath(const QString &imagePath)
{
    if (m_dstImagePath == imagePath)
    {
        return;
    }
    m_dstImagePath = imagePath;
}

QString ImagePreviewWidget::dstImagePath()
{
    return m_dstImagePath;
}

void ImagePreviewWidget::loadSrcImage()
{
    QPixmap pixmap(m_srcImagePath);
    QPoint pixmapLefttop;
    int diam = CUTTING_CIRCLE_RADIUS * 2;

    Q_ASSERT(!pixmap.isNull());

    //图片宽或高小于直径，等比例拉伸到至少可以填满整个圆
    if ((pixmap.width() < diam) || (pixmap.height() < diam))
    {
        pixmap = pixmap.scaled(diam, diam, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }

    m_originalPixmap = m_scaledPixmap = pixmap;
    m_pixmapScaleFactor = 1.0;
    QTimer::singleShot(0, this, SLOT(pixmapCentering()));

    update();
}

void ImagePreviewWidget::resizeEvent(QResizeEvent *event)
{
    if (!imagePositionVerification())
    {
        imageAutomaticPosition();
        update();
    }
    QWidget::resizeEvent(event);
}

bool ImagePreviewWidget::imagePositionVerification()
{
    QRect circleRect((width() - CUTTING_CIRCLE_RADIUS * 2) / 2,
                     (height() - CUTTING_CIRCLE_RADIUS * 2) / 2,
                     CUTTING_CIRCLE_RADIUS * 2,
                     CUTTING_CIRCLE_RADIUS * 2);
    QRect pixmapRect(m_pixmapLefttop, m_scaledPixmap.size());
    return pixmapRect.contains(circleRect);
}

void ImagePreviewWidget::imageAutomaticPosition()
{
    QRect pixmapRect(m_pixmapLefttop, m_scaledPixmap.size());

    QRect circleRect((width() - CUTTING_CIRCLE_RADIUS * 2) / 2,
                     (height() - CUTTING_CIRCLE_RADIUS * 2) / 2,
                     CUTTING_CIRCLE_RADIUS * 2,
                     CUTTING_CIRCLE_RADIUS * 2);

    if (pixmapRect.left() > circleRect.left())
    {
        pixmapRect.moveLeft(circleRect.left());
    }
    if (pixmapRect.right() < circleRect.right())
    {
        pixmapRect.moveRight(circleRect.right());
    }
    if (pixmapRect.top() > circleRect.top())
    {
        pixmapRect.moveTop(circleRect.top());
    }
    if (pixmapRect.bottom() < circleRect.bottom())
    {
        pixmapRect.moveBottom(circleRect.bottom());
    }

    m_pixmapLefttop = pixmapRect.topLeft();
}

void ImagePreviewWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
    {
        pixmapReduce();
    }
    else
    {
        pixmapEnlarge();
    }
    QWidget::wheelEvent(event);
}

void ImagePreviewWidget::pixmapEnlarge()
{
    double scaleFactor = m_pixmapScaleFactor + SCALE_STEP_VALUE;
    if (scaleFactor > IMAGE_SCALE_FACTOR_MAXIMUM)
    {
        return;
    }
    QSize oriSize = m_originalPixmap.size();
    QPixmap pixmap = m_originalPixmap.scaled(oriSize.width() * scaleFactor,
                                             oriSize.height() * scaleFactor,
                                             Qt::KeepAspectRatioByExpanding,
                                             Qt::SmoothTransformation);
    ///保持缩放之前图片在裁切框中心点的位置不变
    QPoint lefttop(m_pixmapLefttop.x() - (pixmap.width() - m_scaledPixmap.width()) / 2,
                   m_pixmapLefttop.y() - (pixmap.height() - m_scaledPixmap.height()) / 2);

    m_pixmapLefttop = lefttop;
    m_scaledPixmap = pixmap;
    m_pixmapScaleFactor = scaleFactor;
    update();
}

void ImagePreviewWidget::pixmapReduce()
{
    double scaleFactor = m_pixmapScaleFactor - SCALE_STEP_VALUE;
    QSize oriSize = m_originalPixmap.size();
    QPixmap pixmap = m_originalPixmap.scaled(oriSize.width() * scaleFactor,
                                             oriSize.height() * scaleFactor,
                                             Qt::KeepAspectRatioByExpanding,
                                             Qt::SmoothTransformation);
    int circleDiam = CUTTING_CIRCLE_RADIUS * 2;
    if ((pixmap.width() < (circleDiam)) || (pixmap.height() < (circleDiam)))
    {
        return;
    }
    QPoint lefttop(m_pixmapLefttop.x() - (pixmap.width() - m_scaledPixmap.width()) / 2,
                   m_pixmapLefttop.y() - (pixmap.height() - m_scaledPixmap.height()) / 2);
    m_pixmapLefttop = lefttop;
    m_scaledPixmap = pixmap;
    m_pixmapScaleFactor = scaleFactor;
    if (!imagePositionVerification())
    {
        imageAutomaticPosition();
    }
    update();
}

void ImagePreviewWidget::pixmapCentering()
{
    m_pixmapLefttop.setX((width() - m_scaledPixmap.width()) / 2);
    m_pixmapLefttop.setY((height() - m_scaledPixmap.height()) / 2);
    update();
}

bool ImagePreviewWidget::saveAvatar()
{
    QPixmap circleRectPixmap = generateCircleRectPixmap();
    QPixmap avatar = generateCircleAvatar(circleRectPixmap);
    if (avatar.save(m_dstImagePath, "png", 100))
    {
        // 修改文件权限，只允许当前用户进行读写
        // 让AccountServices将用户icon_file的路径修改为文件拷贝之后的
        QFile::setPermissions(m_dstImagePath, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        return true;
    }
    return false;
    ;
}

QPixmap ImagePreviewWidget::generateCircleRectPixmap()
{
    QPixmap pixmap(CUTTING_CIRCLE_RADIUS * 2, CUTTING_CIRCLE_RADIUS * 2);
    pixmap.fill(Qt::transparent);

    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHints(QPainter::HighQualityAntialiasing);

    QRectF circleRect((width() - CUTTING_CIRCLE_RADIUS * 2) / 2.0,
                      (height() - CUTTING_CIRCLE_RADIUS * 2) / 2.0,
                      CUTTING_CIRCLE_RADIUS * 2,
                      CUTTING_CIRCLE_RADIUS * 2);
    QRect pixmapInCircle(circleRect.x() - m_pixmapLefttop.x(),
                         circleRect.y() - m_pixmapLefttop.y(),
                         CUTTING_CIRCLE_RADIUS * 2,
                         CUTTING_CIRCLE_RADIUS * 2);
    pixmapPainter.drawRect(pixmap.rect());
    pixmapPainter.drawPixmap(pixmap.rect(), m_scaledPixmap, pixmapInCircle);
    return pixmap;
}

QPixmap ImagePreviewWidget::generateCircleAvatar(const QPixmap &pixmap)
{
    QBitmap mask(pixmap.size());
    QPainter maskPainter(&mask);
    QPixmap temp = pixmap;

    maskPainter.setRenderHints(QPainter::HighQualityAntialiasing);
    maskPainter.fillRect(0, 0, pixmap.width(), pixmap.height(), Qt::white);
    maskPainter.setBrush(QColor(0, 0, 0));
    maskPainter.drawRoundedRect(0, 0,
                                pixmap.width(), pixmap.height(),
                                pixmap.width() / 2, pixmap.width() / 2);
    temp.setMask(mask);
    return temp;
}
