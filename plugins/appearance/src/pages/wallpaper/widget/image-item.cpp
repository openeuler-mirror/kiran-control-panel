/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "image-item.h"
#include <kiran-log/qt5-log-i.h>
#include "../wallpaper-def.h"
#include "image-load-manager.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QToolButton>
#include <QtConcurrent/QtConcurrent>
#include <utility>

#define IMAGE_ITEM_DEFAULT_WIDTH 180.0
#define IMAGE_ITEM_DEFAULT_HEIGHT 100.0

ImageItem::ImageItem(QWidget *parent, const QString &path, int imageType)
    : QWidget(parent),
      m_imagePath(path),
      m_imageType(imageType)
{
    setAttribute(Qt::WA_Hover, true);
    setToolTip(path.split("/").last());

    if (m_imageType == CUSTOM_IMAGE)
    {
        createDeleteButton();
    }
    if (m_imageType == ADDITION_IMAGE)
    {
        m_isAdditionImage = true;
    }

    connect(ImageLoadManager::instance(), &ImageLoadManager::imageLoaded,
            this, &ImageItem::loadPixmapFinished, Qt::QueuedConnection);
}

ImageItem::~ImageItem()
{
}

void ImageItem::createDeleteButton()
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    m_deleteBtn = new QToolButton(this);
    m_deleteBtn->setObjectName("deleteButton");
    m_deleteBtn->setFixedSize(64, 28);
    m_deleteBtn->setStyleSheet("QToolButton{border:none;background-color:transparent;background-image:url(:/kcp-appearance/images/wallpaper-delete-normal.png);}"
                               "QToolButton:hover{background-image:url(:/kcp-appearance/images/wallpaper-delete-pre.png);}"
                               "QToolButton:pressed{background-image:url(:/kcp-appearance/images/wallpaper-delete-select.png);}");

    hLayout->addWidget(m_deleteBtn, Qt::AlignCenter);
    m_deleteBtn->hide();

    connect(m_deleteBtn, &QToolButton::clicked,
            [=] {
                emit deleteBtnClicked(m_imagePath);
            });
}

void ImageItem::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    bool imageIsLoaded = false;

    if (m_isAdditionImage)
    {
        //draw + background:#2d2d2d,180*100 border:#393939 1px 3,3,-6,-6
        QBrush brush(QColor("#2d2d2d"));
        QRect rectBg(rect().x() + 3, rect().y() + 3, rect().width() - 6, rect().height() - 6);
        p.fillRect(rectBg, brush);

        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor("#393939"));
        p.setPen(pen);
        p.drawRect(rectBg);

        QPoint centerRect = rect().center();
        QLine hLine(centerRect.x() - 20, centerRect.y(), centerRect.x() + 20, centerRect.y());
        QLine vLine(centerRect.x(), centerRect.y() - 20, centerRect.x(), centerRect.y() + 20);

        QPen penLine;
        penLine.setColor(QColor("#ffffff"));
        penLine.setWidth(2);
        p.setPen(penLine);
        p.drawLine(hLine);
        p.drawLine(vLine);
    }
    else
    {
        //NOTE:ImageItem绘制过程由自己绘制不在Style中绘制的原因是由于不想再次图片在内存中再次拷贝传递给Style

        if (m_previewPixmap.first == QSize(180, 100) && !m_previewPixmap.second.isNull())
        {
            QRect drawTargetRect(rect().x() + 3, rect().y() + 3, rect().width() - 6, rect().height() - 6);
            p.drawPixmap(drawTargetRect, m_previewPixmap.second, m_previewPixmap.second.rect());
            imageIsLoaded = true;
        }
        else
        {
            p.fillRect(rect(), QBrush(QColor("#292929")));
            drawLoadingImage(p);
        }

        if (m_isSelected)
        {
            drawSelectedIndicator(p);
        }
        else
        {
            /* 没加载完成不绘制遮罩 */
            if (imageIsLoaded)
            {
                drawMask(p);
            }
            if (m_isHover)
            {
                drawHoverIndicator(p);
            }
        }
    }
}

void ImageItem::mousePressEvent(QMouseEvent *event)
{
    m_isDown = true;
    event->accept();
}

void ImageItem::loadPixmapFinished(QString imagePath, QSize imageSize, QPixmap pixmap)
{
    if (imagePath == m_imagePath)
    {
        m_previewPixmap.first = imageSize;
        m_previewPixmap.second = std::move(pixmap);
        update();
    }
}

void ImageItem::updatePixmap()
{
    if (m_previewPixmap.first == QSize(180, 100) && !m_previewPixmap.first.isNull())
    {
        return;
    }
    if (m_imagePath.isNull())
    {
        KLOG_INFO() << "m_imagepath is null";
        return;
    }
    ImageLoadManager::instance()->load(m_imagePath, QSize(180, 100));
}

void ImageItem::drawSelectedIndicator(QPainter &painter)
{
    painter.save();

    QPen pen(QColor("#2eb3ff"));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(rect());
    painter.restore();
}

void ImageItem::drawHoverIndicator(QPainter &painter)
{
    painter.save();
    QPen pen(QColor(255, 255, 255, 0.3 * 255));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(rect());
    painter.restore();
}

void ImageItem::drawMask(QPainter &painter)
{
    painter.save();
    QBrush brush(QColor(0, 0, 0, 0.5 * 255));
    QRect rect_image(rect().x() + 3, rect().y() + 3, rect().width() - 6, rect().height() - 6);
    painter.fillRect(rect_image, brush);
    painter.restore();
}

QString ImageItem::imagePath()
{
    return m_imagePath;
}

void ImageItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    if (m_isDown)
    {
        setIsSelected(true);
    }

    m_isDown = false;
}

bool ImageItem::isSelected()
{
    return m_isSelected;
}

void ImageItem::setIsSelected(bool selected)
{
    if (!m_isAdditionImage)
    {
        m_isSelected = selected;
        emit isSelectedChanged(m_isSelected);
        if (m_isSelected)
        {
            emit itemIsSelected();
        }
        update();
    }
    else
    {
        emit addItemClicked();
    }
}

void ImageItem::drawLoadingImage(QPainter &painter)
{
    static QSvgRenderer loadingRender(QString(":/kiran-control-panel/images/loading.svg"));
    QRect widgetRect = rect();
    qreal widgetScaledFactor = widgetRect.width() / IMAGE_ITEM_DEFAULT_WIDTH;
    QSize loadingSize(loadingRender.defaultSize().width() * widgetScaledFactor,
                      loadingRender.defaultSize().height() * widgetScaledFactor);
    QRect loadingRect(QPoint((widgetRect.width() - loadingSize.width()) / 2, (widgetRect.height() - loadingSize.height()) / 2),
                      loadingSize);
    loadingRender.render(&painter, loadingRect);
}

void ImageItem::enterEvent(QEvent *event)
{
    m_isHover = true;
    if (m_imageType == CUSTOM_IMAGE)
        m_deleteBtn->show();
    QWidget::enterEvent(event);
}

void ImageItem::leaveEvent(QEvent *event)
{
    m_isHover = false;
    if (m_imageType == CUSTOM_IMAGE)
    {
        m_deleteBtn->hide();
    }
    QWidget::leaveEvent(event);
}
