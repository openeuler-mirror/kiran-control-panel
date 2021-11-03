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

#include "image-selector.h"
#include <kiran-log/qt5-log-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStyleOption>
#include <iostream>
#include "../wallpaper-global.h"
#include "flowlayout.h"
#include "image-item.h"
#include "image-load-manager.h"
#include "scroll-container.h"
ImageSelector::ImageSelector(QWidget *parent) : QWidget(parent)
{
    initUI();
    m_updateTimer.setInterval(200);
    m_updateTimer.setSingleShot(true);
    connect(&m_updateTimer, &QTimer::timeout,
            this, &ImageSelector::updateImageItem);
}

void ImageSelector::initUI()
{
    adjustSize();
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setObjectName("ImageSelector");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
    QWidget *widget = new QWidget(this);
    widget->setObjectName("containerWidget");
    mainLayout->addWidget(widget);

    QVBoxLayout *vLayout = new QVBoxLayout(widget);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vLayout->addWidget(scrollArea);

    m_flowLayout = new FlowLayout(0, 10, 10);
    m_flowLayout->setContentsMargins(10, 10, 10, 10);
    m_container = new ScrollContainer(scrollArea);
    m_container->setObjectName("container");
    m_container->setLayout(m_flowLayout);
    scrollArea->setWidget(m_container);
    setAttribute(Qt::WA_NoSystemBackground, true);

    connect(m_container, &ScrollContainer::resized,
            [=](QSize newSize) {
                int height = m_flowLayout->heightForWidth(newSize.width());
                if (height <= this->height())
                    widget->resize(size().width(), height);
            });
}

bool ImageSelector::isImageExisted(QString path)
{
    for (ImageItem *item : m_itemList)
    {
        if (item->imagePath() == path)
        {
            return true;
        }
    }
    return false;
}

QStringList ImageSelector::imageList()
{
    return m_imageList;
}

void ImageSelector::addImage(const QString &imagePath, int imageType)
{
    if (!imagePath.isNull())
    {
        if (isImageExisted(imagePath))
        {
            //add failed;
            KiranMessageBox::message(nullptr, tr("Add Image Failed"),
                                     tr("The image already exists!"), KiranMessageBox::Ok);
            return;
        }
        m_imageList.append(imagePath);
    }

    ImageItem *item = new ImageItem(this, imagePath, imageType);
    item->setFixedSize(186, 106);
    item->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if (imageType == ADDITION_IMAGE)
    {
        m_addImageItem = item;
    }
    if (!imagePath.isNull())
    {
        m_itemList.append(item);
    }

    m_flowLayout->addWidget(item);
    m_updateTimer.start();

    connect(item, SIGNAL(itemIsSelected()), this, SLOT(handlerImageItemSelectedChanged()));
    connect(item, SIGNAL(deleteBtnClicked(QString)), this, SLOT(handlerImageDelete(QString)));
    connect(item, &ImageItem::addItemClicked,
            [=] {
                emit addNewImage();
            });
}

QString ImageSelector::selectedImage()
{
    return m_selectedImagePath;
}

void ImageSelector::setSelectedImage(QString path, bool)
{
    for (ImageItem *item : m_itemList)
    {
        if (item->imagePath() == path)
        {
            item->setIsSelected(true);
            m_selectedImagePath = path;
            break;
        }
    }
}

void ImageSelector::moveAdditionItemToEnd()
{
    m_flowLayout->removeWidget(m_addImageItem);
    m_flowLayout->addWidget(m_addImageItem);
}

void ImageSelector::setSelectorType(int type)
{
    m_selectorType = type;
}

void ImageSelector::updateImageItem()
{
    ImageLoadManager::instance()->reset();
    for (ImageItem *item : m_itemList)
    {
        item->updatePixmap();
    }
}

void ImageSelector::handlerImageItemSelectedChanged()
{
    ImageItem *senderItem = qobject_cast<ImageItem *>(sender());

    for (ImageItem *item : m_itemList)
    {
        if (item != senderItem)
        {
            item->setIsSelected(false);
        }
    }
    m_selectedImagePath = senderItem->imagePath();

    emit selectedImageChanged(m_selectorType, m_selectedImagePath);
}

void ImageSelector::handlerImageDelete(QString imagePath)
{
    if (!imagePath.isNull())
    {
        KiranMessageBox::KiranStandardButton button = KiranMessageBox::message(nullptr, tr("Delete image"),
                                                                               tr("Are you sure you want to delete this picture?"),
                                                                               KiranMessageBox::Yes | KiranMessageBox::No);
        if (button == KiranMessageBox::Yes)
        {
            auto deletePos = m_itemList.end();
            int deletePosIndex = 0;
            for (QList<ImageItem *>::iterator iter = m_itemList.begin();
                 iter != m_itemList.end();
                 iter++)
            {
                if ((*iter)->imagePath() == imagePath)
                {
                    deletePos = iter;
                    deletePosIndex = m_itemList.indexOf((*deletePos));
                    KLOG_INFO() << "deletePos index = " << deletePosIndex;
                    break;
                }
            }

            if (deletePos != m_itemList.end())
            {
                (*deletePos)->deleteLater();
                QList<ImageItem *>::iterator iterNext = m_itemList.erase(deletePos);

                emit deleteImage(imagePath);
                ///删除已选中的图片,更新选中的图片项
                if (m_selectedImagePath == imagePath)
                {
                    //若不是最后一个，则设置为后一个
                    if (iterNext != m_itemList.end())
                    {
                        KLOG_INFO() << "not last one";
                        (*iterNext)->setIsSelected(true);
                        m_selectedImagePath = (*iterNext)->imagePath();
                    }
                    else  //若是最后一个，则设置为前一个
                    {
                        KLOG_INFO() << "is last one";
                        m_itemList.at(deletePosIndex - 1)->setIsSelected(true);
                        m_selectedImagePath = m_itemList.at(deletePosIndex - 1)->imagePath();
                    }
                    KLOG_INFO() << "current m_selectedImagePath = " << m_selectedImagePath;
                    emit selectedImageChanged(m_selectorType, m_selectedImagePath);
                }
            }
            m_updateTimer.start();
        }
    }
}

void ImageSelector::paintEvent(QPaintEvent *event)
{
    QStyleOption option;
    option.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);
}
