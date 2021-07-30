#include "image-selector.h"
#include "flowlayout.h"
#include "kiran-image-item.h"
#include "kiran-image-load-manager.h"
#include "../wallpaper-global.h"
#include "scroll-container.h"
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QScrollArea>
#include <QStyleOption>
#include <QPainter>
#include <QResizeEvent>
#include <iostream>
ImageSelector::ImageSelector(QWidget *parent):
    QWidget(parent)
{
    initUI();
    m_updateTimer.setInterval(200);
    m_updateTimer.setSingleShot(true);
    connect(&m_updateTimer, &QTimer::timeout,
            this, &ImageSelector::updateImageItem);
}

void ImageSelector::initUI()
{
    //setMinimumHeight(470);
    adjustSize();
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    setObjectName("ImageSelector");
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    setLayout(vLayout);

    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vLayout->addWidget(scrollArea);

    m_flowLayout = new FlowLayout(0,10,10);
    m_flowLayout->setContentsMargins(10,10,10,10);
    m_container = new ScrollContainer(scrollArea);
    m_container->setObjectName("container");
    m_container->setMinimumWidth(610);
    m_container->setLayout(m_flowLayout);
    scrollArea->setWidget(m_container);
//    setLayout(m_flowLayout);
//    setMinimumWidth(610);
    setAttribute(Qt::WA_NoSystemBackground,true);

    connect(m_container,&ScrollContainer::resized,
            [=](QSize newSize){
        std::cout << "size = " <<
                     newSize.width() << ","
                  << newSize.height() << std::endl;
        int height = m_flowLayout->heightForWidth(newSize.width());
        resize(newSize.width(),height);
    });
}

bool ImageSelector::isImageExisted(QString path)
{
    for(KiranImageItem *item:m_itemList)
    {
        if(item->imagePath() == path)
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
    if(!imagePath.isNull())
    {
        if(isImageExisted(imagePath))
        {
            //add failed;
            KiranMessageBox::message(nullptr,tr("Add Image Failed"),
                                     tr("The image already exists!"),KiranMessageBox::Ok);
            return;
        }
        m_imageList.append(imagePath);
    }

    KiranImageItem * item = new KiranImageItem(this,imagePath,imageType);
    item->setFixedSize(186,106);
    item->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if(imageType == ADDITION_IMAGE)
    {
        m_addImageItem = item;
    }
    if(!imagePath.isNull())
    {
        m_itemList.append(item);
    }

    m_flowLayout->addWidget(item);
    m_updateTimer.start();

    connect(item,SIGNAL(itemIsSelected()),this,SLOT(handlerImageItemSelectedChanged()));
    connect(item,SIGNAL(deleteBtnClicked(QString)),this,SLOT(handlerImageDelete(QString)));
    connect(item,&KiranImageItem::addItemClicked,
            [=]{
        emit addNewImage();
    });
}

void ImageSelector::removeImage(QString path)
{

}

QString ImageSelector::selectedImage()
{
    return m_selectedImagePath;
}

void ImageSelector::setSelectedImage(QString path, bool)
{
    for(KiranImageItem *item:m_itemList)
    {
        if(item->imagePath() == path)
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
    KiranImageLoadManager::instance()->reset();
    for (KiranImageItem *item:m_itemList) {     
            item->updatePixmap();
    }
}

void ImageSelector::handlerImageItemSelectedChanged()
{
    KiranImageItem *senderItem = qobject_cast<KiranImageItem *>(sender());

//    if(!isAdditionImage)
//    {
        for (KiranImageItem *item:m_itemList) {
            if (item != senderItem) {
                item->setIsSelected(false);
            }
        }
        m_selectedImagePath = senderItem->imagePath();
    //}
    emit selectedImageChanged(m_selectorType ,m_selectedImagePath);
}

void ImageSelector::handlerImageDelete(QString imagePath)
{
//    for(KiranImageItem *item : m_itemList)
//        std::cout << "pre itemList: size:" << m_itemList.size() << "image:"<< item->imagePath().toStdString() << std::endl;

//    for(QString path:m_imageList)
//        std::cout << "pre imageList: size:" << m_imageList.size() << "image:" <<path.toStdString() << std::endl;

//    std::cout << "imagePath = " << imagePath.toStdString() << std::endl;

    if(!imagePath.isNull())
    {
        KiranMessageBox::KiranStandardButton button = KiranMessageBox::message(nullptr,tr("Delete image"),
                                                                               tr("Are you sure you want to delete this picture?"),
                                                                               KiranMessageBox::Yes| KiranMessageBox::No);
        if(button == KiranMessageBox::Yes)
        {
            auto deletePos = m_itemList.end();
            int deletePosIndex = 0;
            for( QList<KiranImageItem*>::iterator iter=m_itemList.begin();
                 iter!=m_itemList.end();
                 iter++){
                if( (*iter)->imagePath() == imagePath ){
                    deletePos = iter;
                    deletePosIndex = m_itemList.indexOf((*deletePos));
                    std::cout << "deletePos index = " << deletePosIndex << std::endl;
                    break;
                }
            }

            if( deletePos != m_itemList.end() ){
                (*deletePos)->deleteLater();
                QList<KiranImageItem *>::iterator iterNext = m_itemList.erase(deletePos);
//                std::cout <<  "iterNext path = " << (*iterNext)->imagePath().toStdString() << std::endl;
//                std::cout << "m_selectedImagePath = " << m_selectedImagePath.toStdString() << std::endl;
//                std::cout << "(*deletePos)->imagePath = "  << (*deletePos)->imagePath().toStdString() << std::endl;

                emit deleteImage(imagePath);
                ///删除已选中的图片,更新选中的图片项
                if( m_selectedImagePath==imagePath ){
                    //若不是最后一个，则设置为后一个
                    if(iterNext != m_itemList.end())
                    {
                        std::cout << "not last one" << std::endl;
                        (*iterNext)->setIsSelected(true);
                        m_selectedImagePath = (*iterNext)->imagePath();
                    }
                    else //若是最后一个，则设置为前一个
                    {
                        std::cout << "is last one" << std::endl;
                        m_itemList.at(deletePosIndex-1)->setIsSelected(true);
                        m_selectedImagePath = m_itemList.at(deletePosIndex-1)->imagePath();
                    }
                    std::cout << "current m_selectedImagePath = " << m_selectedImagePath.toStdString() << std::endl;
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

//void ImageSelector::resizeEvent(QResizeEvent *event)
//{
//    std::cout << "size = " <<
//                 this->geometry().width() << ","
//              << this->geometry().height() << std::endl;
//    QSize size = event->size();
//    int height = m_flowLayout->heightForWidth(size.width());
//    resize(size.width(),height);
//}
