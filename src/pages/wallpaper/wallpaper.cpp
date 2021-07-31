#include "wallpaper.h"
#include "ui_wallpaper.h"
/*
背景设置设计思路
一、总体：
分两个页面：主题设置主界面(Wallpaper)、壁纸选择界面(ImageSelector)（多线程创建，参考/home/yuanxing/kiranwidgets-qt5/src/widgets/kiran-image-selector/kiran-image-load-manager.cpp代码）
关联：用户点击选择壁纸widget，切换到壁纸选择界面
     用户点击壁纸，返回主界面，并且根据选择的壁纸类型（桌面/锁屏），更新预览图中的背景

前期工作：
枚举定义两种壁纸类型，供参数传递
1、创建壁纸预览控件类，分为桌面壁纸预览和锁屏壁纸预览两类，每个类包括预览图片以及预览类型名字
2、创建壁纸选择btn，提供setIcon、getIconPath接口，以及根据按钮的鼠标选择、悬浮状态，改变样式，
        其中鼠标悬浮时，在自定义图片btn中添加删除btn，删除btn分三种状态：选中、悬浮、普通

二、主界面
1、获取当前的桌面壁纸path、锁屏壁纸path
2、创建两个预览控件，将当前的path对应的图片绘至预览控件，当做背景
3、将预览控件添加至主界面
4、创建壁纸选择控件，创建两个widget，监听clicked信号，（信号中传入壁纸类型, 和对应的壁纸路径）跳转至壁纸选择界面，同时根据当前的壁纸名字，设置对应的壁纸btn的 icon

三、壁纸选择界面
1、创建一个壁纸选择控件（QWidget），放到scrollArea里面
2、在壁纸选择控件中添加FlowLayout
3、加载系统中的壁纸图片，获取绝对路径，加载用户目录下的.config/kylinsec/background.xml文件（先判断有没有，只有在用户添加了自己的图片进去才会有该文件，若有的话，就加载）
4、创建自定义的btn类，设置btn的icon(setIcon,getPath)
5、将btn添加进btnGroup
6、添加+btn
7、若点击+btn，则弹出文件管理器，供用户选择，选择后，将对应的图片添加xml，并创建壁纸btn，添加进btnGroup
               确保+btn在界面的最后
8、若点击删除按钮，则弹出删除确认消息框，用户点击确认后，删除图片（删除图片文件，删除btn）,如果删除的图片刚好是当前设置的壁纸，则×××××××
9、若选中壁纸，则获取壁纸btn的path，以及壁纸名字，发送设置是否成功信号，传入path,壁纸类型，返回主界面
10、主界面壁纸设置监听信号，成功后，更新选择控件的name，根据传来的path，与壁纸类型，更新对应预览控件背景


问题：
1、获取和设置锁屏壁纸的接口！(后续会添加锁屏壁纸设置接口)
2、从哪里获取壁纸图片(系统路径和用户自定义的图片存储的路径（～/.config/kylinsec/background.xml）)
3、添加或删除壁纸图片，要进行什么操作，比如将图片从哪里删除了，或者添加到了哪里 (只能删除用户自添加的图片，更新～/.config/kylinsec/background.xml文件)
4、如果删除的刚好是正设置的壁纸，要默认选择哪个壁纸返回（默认default.jpg）
5、如果只是添加或删除壁纸，那么界面如何返回，是否要加返回按钮（直接点击左侧列表中的背景设置）

xml文件图片信息项:(read,set)
    deleted: true or false
    path
*/

#include "dbus-interface/appearance-global-info.h"
#include "widget/preview-label.h"
#include "common/chooser-widget.h"
#include "wallpaper-global.h"
#include "widget/flowlayout.h"
#include "widget/image-selector.h"
#include "widget/kiran-image-load-manager.h"
#include "widget/xml-management/thread-object.h"
#include <kiran-message-box.h>
#include <kiran-style-public-define.h>
#include <QFileDialog>
#include <QDir>
#include <QThread>
#include <iostream>


Wallpaper::Wallpaper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Wallpaper)
{
    ui->setupUi(this);
    initUI();
    createPreviewLabel();

    createChooserWidget();

    handleImageSelector();
}

Wallpaper::~Wallpaper()
{
    delete ui;
}

void Wallpaper::initUI()
{
    ui->stackedWidget->setCurrentIndex(0);
    m_imageSelector = new ImageSelector(this);
    ui->vLayout_image_selector->addWidget(m_imageSelector);
}

void Wallpaper::createPreviewLabel()
{
    m_currDesktopWp = AppearanceGlobalInfo::instance()->getDesktopBackground();
    m_currLockScreenWp = AppearanceGlobalInfo::instance()->getLockScreenBackground();

    QLayout *layoutDesktop = ui->widget_desktop_preview->layout();
    PreviewLabel *desktopPreview =  new PreviewLabel(DESKTOP,m_currDesktopWp,this);
    layoutDesktop->addWidget(desktopPreview);
    layoutDesktop->setAlignment(desktopPreview,Qt::AlignHCenter);
    connect(this,&Wallpaper::wallpaperChanged,
            [=](int type, QString path){
        if(type == DESKTOP)
            desktopPreview->updateWallpaper(DESKTOP,path);
    });

    QLayout *layout = ui->widget_lockscreen_preview->layout();
    PreviewLabel *lockScreenPreview = new PreviewLabel(LOCK_SCREEN,m_currLockScreenWp,this);
    layout->addWidget(lockScreenPreview);
    layout->setAlignment(lockScreenPreview,Qt::AlignHCenter);
    connect(this,&Wallpaper::wallpaperChanged,
            [=](int type, QString path){
        if(type == LOCK_SCREEN)
            lockScreenPreview->updateWallpaper(LOCK_SCREEN,path);
    });
}

void Wallpaper::createChooserWidget()
{
    QString desktopWpName = m_currDesktopWp.split("/").last();
    QString lockScreenWpName = m_currLockScreenWp.split("/").last();

    m_desktopWpChooser = new ChooserWidget(tr("Set Desktop Wallpaper"),DESKTOP);
    m_desktopWpChooser->setName(desktopWpName);
    ui->vLayout_chooser->addWidget(m_desktopWpChooser);
    connect(m_desktopWpChooser,&ChooserWidget::clicked,
            [=]{
        m_imageSelector->setSelectorType(DESKTOP);
        m_imageSelector->setSelectedImage(m_currDesktopWp,false);
        ui->stackedWidget->setCurrentIndex(1);
    });

    m_lockScreenWPChooser = new ChooserWidget(tr("Set Lock Screen Wallpaper"),LOCK_SCREEN);
    m_lockScreenWPChooser->setName(lockScreenWpName);
    ui->vLayout_chooser->addWidget(m_lockScreenWPChooser);
    connect(m_lockScreenWPChooser,&ChooserWidget::clicked,
            [=]{
       m_imageSelector->setSelectorType(LOCK_SCREEN);
       m_imageSelector->setSelectedImage(m_currLockScreenWp,false);
       ui->stackedWidget->setCurrentIndex(1);
    });

}

void Wallpaper::handleImageSelector()
{
    loadVisibleWallpapers();

    connect(m_imageSelector, &ImageSelector::selectedImageChanged,
            [=](int type, QString imagePath){
        if(!imagePath.isNull())
        {
            std::cout << "image: " << imagePath.toStdString() << std::endl;
            //set background
            if(type == DESKTOP)
            {
                if(m_currDesktopWp == imagePath)
                {
                    ui->stackedWidget->setCurrentIndex(0);
                    return ;
                }
                if(AppearanceGlobalInfo::instance()->setDesktopBackground(imagePath))
                {
                    m_desktopWpChooser->setName(imagePath.split("/").last());
                    m_currDesktopWp = imagePath;
                    ui->stackedWidget->setCurrentIndex(0);
                    emit wallpaperChanged(type,imagePath);
                }
                else
                    KiranMessageBox::message(nullptr,tr("set wallpaper"),tr("Set wallpaper failed!"),KiranMessageBox::Ok);

            }
            else
            {
                if(m_currLockScreenWp == imagePath)
                {
                    ui->stackedWidget->setCurrentIndex(0);
                    return ;
                }
                if(AppearanceGlobalInfo::instance()->setLockScreenBackground(imagePath))
                {
                    m_lockScreenWPChooser->setName(imagePath.split("/").last());
                    m_currLockScreenWp = imagePath;
                    ui->stackedWidget->setCurrentIndex(0);
                    emit wallpaperChanged(type,imagePath);;
                }
                else
                    KiranMessageBox::message(nullptr,tr("set wallpaper"),tr("Set wallpaper failed!"),KiranMessageBox::Ok);
            }
            //jump to main ui
        }
    });
    connect(m_imageSelector,&ImageSelector::addNewImage,
            [=]{
        bool flag = false;
        QString fileName = QFileDialog::getOpenFileName(this, tr("select picture"),
                                                        QDir::homePath(),
                                                        tr("image files(*.bmp *.jpg *.png *.tif *.gif"
                                                           " *.pcx *.tga *.exif *.fpx *.svg *.psd *.cdr *.pcd"
                                                           " *.dxf *.ufo *.eps *.ai *.raw *.WMF *.webp)"));
        if (fileName.isEmpty()) {
            return;
        }
        //select
        //addImage
        m_imageSelector->addImage(fileName,CUSTOM_IMAGE);
        //move additionImage Item to end
        m_imageSelector->moveAdditionItemToEnd();

        // 将图片信息存储在xml文件中，若文件不存在则创建
        for( QList<QMap<QString,QString>>::iterator iter=m_wallpaperMapList.begin();
             iter!=m_wallpaperMapList.end();
             iter++){
            if((*iter).find("filename").value() == fileName)
            {
                flag = true;
                (*iter).insert("deleted","false");
                break;
            }
        }
        if(!flag)
        {
            QMap<QString, QString> newWallpaperInfo;
            newWallpaperInfo.insert("deleted", "false");
            newWallpaperInfo.insert("name", fileName.split("/").last());
            newWallpaperInfo.insert("filename", fileName);
            newWallpaperInfo.insert("artist", "(none)");
            newWallpaperInfo.insert("options", "zoom");
            newWallpaperInfo.insert("pcolor", "#000000");
            newWallpaperInfo.insert("scolor", "#000000");
            newWallpaperInfo.insert("shade_type", "vertical-gradient");
            m_wallpaperMapList.append(newWallpaperInfo);
        }
        m_threadObject->updateWallpaperXml(m_wallpaperMapList);
    });

    connect(m_imageSelector,&ImageSelector::deleteImage,
            [=](QString deletedPath){
        for( QList<QMap<QString,QString>>::iterator iter=m_wallpaperMapList.begin();
             iter!=m_wallpaperMapList.end();
             iter++){
            if((*iter).find("filename").value() == deletedPath)
            {
                (*iter).insert("deleted","true");
                m_threadObject->updateWallpaperXml(m_wallpaperMapList);
                break;
            }
        }
    });

}

void Wallpaper::handleWallpaperInfo(QList<QMap<QString, QString> > wallpaperMapList)
{
    std::cout << "handleWallpaperInfo" << std::endl ;
    if(wallpaperMapList.size()  < 1)
    {
        std::cout << "get wallpaperMap failed" << std::endl ;
        return ;
    }
    m_wallpaperMapList = wallpaperMapList;
    for(QMap<QString,QString> map:wallpaperMapList)
    {
              QString deleted = map.find("deleted").value();
              QString visibleImage;
              if(deleted == "false")
              {
                  visibleImage = map.find("filename").value();
                  std::cout << "see deleted filename" << map.find("filename").value().toStdString() << std::endl ;
                  if(visibleImage.startsWith(SYSTEM_BACKGROUND_PATH))
                      m_imageSelector->addImage(visibleImage,SYSTEM_IMAGE);
                  else
                      m_imageSelector->addImage(visibleImage,CUSTOM_IMAGE);

                  m_visibleWallpaper.append(map.find("filename").value());
              }
    }
    //添加+项
    m_imageSelector->addImage(nullptr,ADDITION_IMAGE);
}

QSize Wallpaper::sizeHint()
{
    return QSize(635,630);
}

void Wallpaper::loadVisibleWallpapers()
{
    m_thread = new QThread;
    m_threadObject = new ThreadObject;
    m_threadObject->moveToThread(m_thread);

    connect(m_threadObject,SIGNAL(getWallpaperInfo(QList<QMap<QString,QString> >)),this,
            SLOT(handleWallpaperInfo(QList<QMap<QString,QString> >)));
     //终止线程deleteLater
    connect(m_thread,SIGNAL(finished()),m_threadObject,SLOT(deleteLater()));
    connect(m_thread,SIGNAL(started()),m_threadObject,SLOT(loadWallpaperInfo()));

    m_thread->start();
}
















