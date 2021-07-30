//
// Created by lxh on 2021/1/15.
//

#include "kiran-image-item.h"
#include "kiran-image-load-manager.h"
#include "../wallpaper-global.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <utility>
#include <QMouseEvent>

//#include "style.h"
#include <QStyleOption>
#include <QPainter>
#include <QHBoxLayout>
#include <QToolButton>


#include <iostream>

#define IMAGE_ITEM_DEFAULT_WIDTH    180.0
#define IMAGE_ITEM_DEFAULT_HEIGHT   100.0

KiranImageItem::KiranImageItem(QWidget *parent, const QString &path, int imageType)
        : QWidget(parent),
          m_imagePath(path),
          m_imageType(imageType)
{
    setAttribute(Qt::WA_Hover,true);
    setToolTip(path);

    if(m_imageType == CUSTOM_IMAGE)
    {
        createDeleteButton();
    }

    setProperty("imageType", imageType);
    setProperty("isAdditionImage",false);
    if(m_imageType == ADDITION_IMAGE)
    {
        m_isAdditionImage = true;
        setProperty("isAdditionImage",true);
    }

    connect(KiranImageLoadManager::instance(), &KiranImageLoadManager::imageLoaded,
            this, &KiranImageItem::loadPixmapFinished, Qt::QueuedConnection);
}

KiranImageItem::~KiranImageItem() {

}

void KiranImageItem::createDeleteButton()
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    m_deleteBtn  = new QToolButton(this);
    m_deleteBtn->setObjectName("deleteButton");
    m_deleteBtn->setFixedSize(64,28);

    hLayout->addWidget(m_deleteBtn,Qt::AlignCenter);
    m_deleteBtn->hide();
    
    connect(m_deleteBtn,&QToolButton::clicked,
            [=]{
        emit deleteBtnClicked(m_imagePath);
    });
}

void KiranImageItem::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    bool imageIsLoaded = false;
    int imageType = property("imageType").toInt();

    if(m_isAdditionImage)
    {
        //draw + background:#2d2d2d,180*100 border:#393939 1px 3,3,-6,-6
        QBrush brush(QColor("#2d2d2d"));
        QRect  rectBg(rect().x()+3,rect().y()+3,rect().width()-6,rect().height()-6);
        p.fillRect(rectBg, brush);

        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor("#393939"));
        p.setPen(pen);
        p.drawRect(rectBg);

        QPoint centerRect = rect().center();
        QLine hLine(centerRect.x()-20,centerRect.y(),centerRect.x()+20,centerRect.y());
        QLine vLine(centerRect.x(),centerRect.y()-20,centerRect.x(),centerRect.y()+20);

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
        //std::cout << "size:" << geometry().size().width() << "," << geometry().size().height() << std::endl;
//        std::cout << "m_previewPixmap.first:" << m_previewPixmap.first.width() << ","
//                  << m_previewPixmap.first.height() << std::endl;

        if (m_previewPixmap.first == QSize(180,100) && !m_previewPixmap.second.isNull()) {
            QRect drawTargetRect(rect().x()+3,rect().y()+3,rect().width()-6,rect().height()-6);
            p.drawPixmap(drawTargetRect, m_previewPixmap.second, m_previewPixmap.second.rect());
            imageIsLoaded = true;
        } else {
            p.fillRect(rect(), QBrush(QColor("#292929")));
            drawLoadingImage(p);
        }

        if (m_isSelected) {
            //std::cout << "painevent: " <<  m_isSelected << std::endl;
            drawSelectedIndicator(p);
        } else {
            /* 没加载完成不绘制遮罩 */
            if (imageIsLoaded) {
                drawMask(p);
            }
            if ( m_isHover ){
                drawHoverIndicator(p);
            }
        }
    }
}

void KiranImageItem::mousePressEvent(QMouseEvent *event) {
    m_isDown = true;
    event->accept();
}

QSize KiranImageItem::sizeHint() const {
    return QSize(212, 148);
}

void KiranImageItem::loadPixmapFinished(QString imagePath, QSize imageSize, QPixmap pixmap) {
    if (imagePath == m_imagePath) {
        m_previewPixmap.first = imageSize;
        m_previewPixmap.second = std::move(pixmap);
        update();
    }
}

void KiranImageItem::updatePixmap() {
    if (m_previewPixmap.first == QSize(180,100) && !m_previewPixmap.first.isNull()) {
        return;
    }
    if(m_imagePath.isNull())
    {
        std::cout  << "m_imagepath is null" << std::endl;
        return ;
    }
    KiranImageLoadManager::instance()->load(m_imagePath, QSize(180,100));
}

void KiranImageItem::drawSelectedIndicator(QPainter &painter) {
    //std::cout << "drawSelectedIndicator:" << m_isSelected << std::endl;
    painter.save();

    QPen pen(QColor("#2eb3ff"));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(rect());
    painter.restore();
}

void KiranImageItem::drawHoverIndicator(QPainter &painter) {
    painter.save();
    QPen pen(QColor(255,255,255,0.3*255));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(rect());
    painter.restore();
}

void KiranImageItem::drawMask(QPainter &painter) {
    painter.save();
    QBrush brush(QColor(0, 0, 0, 0.5 * 255));
    QRect  rect_image(rect().x()+3,rect().y()+3,rect().width()-6,rect().height()-6);
    painter.fillRect(rect_image, brush);
    painter.restore();
}

QString KiranImageItem::imagePath() {
    return m_imagePath;
}

void KiranImageItem::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }
    if (m_isDown) {
        setIsSelected(true);
    }

    m_isDown = false;
}

bool KiranImageItem::isSelected() {
    return m_isSelected;
}

void KiranImageItem::setIsSelected(bool selected) {
    std::cout << "selectd = " << selected << std::endl;
    if (m_isSelected != selected && !m_isAdditionImage) {
        std::cout << "m_isSelected = " << m_isSelected << std::endl;
        m_isSelected = selected;
        emit isSelectedChanged(m_isSelected);
        if (m_isSelected) {
            emit itemIsSelected();
        }
        update();
    }
    if(m_isAdditionImage)
    {
        emit addItemClicked();
    }
}

void KiranImageItem::drawLoadingImage(QPainter &painter) {
    static QSvgRenderer loadingRender(QString(":/images/loading.svg"));
    QRect widgetRect = rect();
    qreal widgetScaledFactor = widgetRect.width() / IMAGE_ITEM_DEFAULT_WIDTH;
    QSize loadingSize(loadingRender.defaultSize().width() * widgetScaledFactor,
                      loadingRender.defaultSize().height() * widgetScaledFactor);
    QRect loadingRect(QPoint((widgetRect.width() - loadingSize.width()) / 2, (widgetRect.height() - loadingSize.height()) / 2),
            loadingSize);
    loadingRender.render(&painter, loadingRect);
}

void KiranImageItem::enterEvent(QEvent *event) {
    m_isHover = true;
    if(m_imageType == CUSTOM_IMAGE)
        m_deleteBtn->show();
    QWidget::enterEvent(event);
}

void KiranImageItem::leaveEvent(QEvent *event) {
    m_isHover = false;
    if(m_imageType == CUSTOM_IMAGE)
    {
        m_deleteBtn->hide();
    }
    QWidget::leaveEvent(event);
}
