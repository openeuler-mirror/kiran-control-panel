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

#include "wallpaper.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <QDir>
#include <QFileDialog>
#include <QThread>
#include "common/chooser-widget.h"
#include "dbus-interface/appearance-global-info.h"
#include "ui_wallpaper.h"
#include "wallpaper-global.h"
#include "widget/flowlayout.h"
#include "widget/image-load-manager.h"
#include "widget/image-selector.h"
#include "widget/preview-label.h"
#include "widget/xml-management/thread-object.h"

#define CACHE_IMAGE_DIR ".config/kylinsec/kiran-cpanel-appearance/"
Wallpaper::Wallpaper(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::Wallpaper)
{
    ui->setupUi(this);

    m_cacheDirName = QString("%1/%2").arg(QDir::homePath()).arg(CACHE_IMAGE_DIR);
    QDir dstDir(m_cacheDirName);
    if (!dstDir.exists())
        dstDir.mkdir(m_cacheDirName);

    initUI();
    createPreviewLabel();

    createChooserWidget();

    handleImageSelector();

    connect(AppearanceGlobalInfo::instance(), &AppearanceGlobalInfo::desktopBackgroundChanged,
            [this](const QString &value) {
                m_desktopPreview->updateWallpaper(DESKTOP, value);
                m_desktopWpChooser->setName(value.split("/").last());
                m_currDesktopWp = value;
            });
    connect(AppearanceGlobalInfo::instance(), &AppearanceGlobalInfo::lockScreenBackgroundChanged,
            [this](const QString &value) {
                m_lockScreenPreview->updateWallpaper(LOCK_SCREEN, value);
                m_lockScreenWPChooser->setName(value.split("/").last());
                m_currLockScreenWp = value;
            });
}

Wallpaper::~Wallpaper()
{
    delete ui;
    m_thread->quit();
    m_thread->wait();
}

void Wallpaper::initUI()
{
    ui->stackedWidget_Wallpaper->setCurrentIndex(0);
    m_imageSelector = new ImageSelector(this);
    ui->vLayout_image_selector->addWidget(m_imageSelector);
}

/**
 * @brief Wallpaper::createPreviewLabel 创建壁纸预览控件
 */
void Wallpaper::createPreviewLabel()
{
    m_currDesktopWp = AppearanceGlobalInfo::instance()->getDesktopBackground();
    m_currLockScreenWp = AppearanceGlobalInfo::instance()->getLockScreenBackground();

    QString drawDesktopImg = getDrawImgName(m_currDesktopWp);
    QLayout *layoutDesktop = ui->widget_desktop_preview->layout();
    m_desktopPreview = new PreviewLabel(DESKTOP, drawDesktopImg, this);
    layoutDesktop->addWidget(m_desktopPreview);
    layoutDesktop->setAlignment(m_desktopPreview, Qt::AlignHCenter);

    QString drawLockScreenImg = getDrawImgName(m_currLockScreenWp);
    QLayout *layout = ui->widget_lockscreen_preview->layout();
    m_lockScreenPreview = new PreviewLabel(LOCK_SCREEN, drawLockScreenImg, this);
    layout->addWidget(m_lockScreenPreview);
    layout->setAlignment(m_lockScreenPreview, Qt::AlignHCenter);
}

/**
 * @brief Wallpaper::createChooserWidget  创建壁纸选择控件
 */
void Wallpaper::createChooserWidget()
{
    m_desktopWpChooser = new ChooserWidget(tr("Set Desktop Wallpaper"), DESKTOP);
    ui->vLayout_chooser->addWidget(m_desktopWpChooser);
    if (m_currDesktopWp.isNull())
        m_desktopWpChooser->setDisabled(true);
    else
    {
        QString desktopWpName = m_currDesktopWp.split("/").last();
        m_desktopWpChooser->setName(desktopWpName);
        connect(m_desktopWpChooser, &ChooserWidget::clicked,
                [=] {
                    m_imageSelector->setSelectorType(DESKTOP);
                    m_imageSelector->setSelectedImage(m_currDesktopWp, false);
                    ui->stackedWidget_Wallpaper->setCurrentIndex(1);
                });
    }

    m_lockScreenWPChooser = new ChooserWidget(tr("Set Lock Screen Wallpaper"), LOCK_SCREEN);
    ui->vLayout_chooser->addWidget(m_lockScreenWPChooser);
    if (m_currLockScreenWp.isNull())
        m_lockScreenWPChooser->setDisabled(true);
    else
    {
        QString lockScreenWpName = m_currLockScreenWp.split("/").last();
        m_lockScreenWPChooser->setName(lockScreenWpName);

        connect(m_lockScreenWPChooser, &ChooserWidget::clicked,
                [=] {
                    m_imageSelector->setSelectorType(LOCK_SCREEN);
                    m_imageSelector->setSelectedImage(m_currLockScreenWp, false);
                    ui->stackedWidget_Wallpaper->setCurrentIndex(1);
                });
    }
}

void Wallpaper::handleImageSelector()
{
    // 加载壁纸信息
    loadVisibleWallpapers();

    //处理点击壁纸信号
    connect(m_imageSelector, &ImageSelector::selectedImageChanged,
            [=](int type, QString imagePath) {
                if (!imagePath.isNull())
                {
                    KLOG_INFO() << "selected image: " << imagePath;
                    //set background
                    if (type == DESKTOP)
                    {
                        if (m_currDesktopWp == imagePath)
                        {
                            ui->stackedWidget_Wallpaper->setCurrentIndex(0);
                            return;
                        }
                        if (AppearanceGlobalInfo::instance()->setDesktopBackground(imagePath))
                            ui->stackedWidget_Wallpaper->setCurrentIndex(0);
                        else
                            KiranMessageBox::message(nullptr, tr("set wallpaper"), tr("Set wallpaper failed!"), KiranMessageBox::Ok);
                    }
                    else
                    {
                        if (m_currLockScreenWp == imagePath)
                        {
                            ui->stackedWidget_Wallpaper->setCurrentIndex(0);
                            return;
                        }
                        if (AppearanceGlobalInfo::instance()->setLockScreenBackground(imagePath))
                            ui->stackedWidget_Wallpaper->setCurrentIndex(0);
                        else
                            KiranMessageBox::message(nullptr, tr("set wallpaper"), tr("Set wallpaper failed!"), KiranMessageBox::Ok);
                    }
                    //jump to main ui
                }
            });

    //添加壁纸
    connect(m_imageSelector, &ImageSelector::addNewImage,
            [=] {
                bool flag = false;
                QString fileName = QFileDialog::getOpenFileName(this, tr("select picture"),
                                                                QDir::homePath(),
                                                                tr("image files(*.bmp *.jpg *.png *.tif *.gif"
                                                                   " *.pcx *.tga *.exif *.fpx *.svg *.psd *.cdr *.pcd"
                                                                   " *.dxf *.ufo *.eps *.ai *.raw *.WMF *.webp)"));
                if (fileName.isEmpty())
                {
                    return;
                }
                //copy
                QFile file(fileName);
                QString dstName = convertImgName(fileName);
                file.copy(QString("%1%2").arg(m_cacheDirName).arg(dstName));

                //addImage
                m_imageSelector->addImage(fileName, CUSTOM_IMAGE);

                //move additionImage Item to ends
                m_imageSelector->moveAdditionItemToEnd();

                // 将图片信息存储在xml文件中，若文件不存在则创建
                for (QList<QMap<QString, QString>>::iterator iter = m_wallpaperMapList.begin();
                     iter != m_wallpaperMapList.end();
                     iter++)
                {
                    if ((*iter).find("filename").value() == fileName)
                    {
                        flag = true;
                        (*iter).insert("deleted", "false");
                        break;
                    }
                }
                if (!flag)
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

    //删除壁纸
    connect(m_imageSelector, &ImageSelector::deleteImage,
            [=](QString deletedPath) {
                for (QList<QMap<QString, QString>>::iterator iter = m_wallpaperMapList.begin();
                     iter != m_wallpaperMapList.end();
                     iter++)
                {
                    if ((*iter).find("filename").value() == deletedPath)
                    {
                        (*iter).insert("deleted", "true");
                        m_threadObject->updateWallpaperXml(m_wallpaperMapList);
                        break;
                    }
                }
            });
}

/**
 * @brief Wallpaper::handleWallpaperInfo 获取显示的壁纸，并添加到壁纸选择器中
 * @param wallpaperMapList  存储壁纸信息的列表
 */
void Wallpaper::handleWallpaperInfo(QList<QMap<QString, QString>> wallpaperMapList)
{
    if (wallpaperMapList.size() < 1)
    {
        KLOG_DEBUG() << "get wallpaperMap failed";
        return;
    }
    m_wallpaperMapList = wallpaperMapList;
    for (QMap<QString, QString> map : wallpaperMapList)
    {
        QString deleted = map.find("deleted").value();
        QString visibleImage;
        if (deleted == "false")
        {
            visibleImage = map.find("filename").value();
            KLOG_INFO() << "visible filename: " << map.find("filename").value();
            //判断背景图片是否存在
            QFile file(visibleImage);

            if (visibleImage.startsWith(SYSTEM_BACKGROUND_PATH))
            {
                if (file.exists())
                    m_imageSelector->addImage(visibleImage, SYSTEM_IMAGE);
            }
            else
            {
                if (file.exists())
                {
                    m_imageSelector->addImage(visibleImage, CUSTOM_IMAGE);

                    QString imgName = convertImgName(visibleImage);
                    QString cacheImg(QString("%1%2").arg(m_cacheDirName).arg(imgName));
                    file.setFileName(cacheImg);
                    if (!file.exists())
                    {
                        //copy
                        KLOG_INFO() << "copy: " << cacheImg;
                        file.copy(visibleImage, cacheImg);
                    }
                }
                else
                {
                    KLOG_INFO() << " search from cache dir";
                    QString imgName = convertImgName(visibleImage);
                    QString cacheImg(QString("%1%2").arg(m_cacheDirName).arg(imgName));
                    file.setFileName(cacheImg);
                    if (file.exists())
                        m_imageSelector->addImage(cacheImg, CUSTOM_IMAGE);
                }
            }
        }
    }
    //添加+项
    m_imageSelector->addImage(nullptr, ADDITION_IMAGE);
}

QSize Wallpaper::sizeHint() const
{
    return QSize(670, 730);
}

/**
 * @brief Wallpaper::loadVisibleWallpapers 创建线程加载壁纸信息
 */
void Wallpaper::loadVisibleWallpapers()
{
    m_thread = new QThread;
    m_threadObject = new ThreadObject;
    m_threadObject->moveToThread(m_thread);

    connect(m_threadObject, SIGNAL(getWallpaperInfo(QList<QMap<QString, QString>>)), this,
            SLOT(handleWallpaperInfo(QList<QMap<QString, QString>>)));
    //终止线程deleteLater
    connect(m_thread, SIGNAL(finished()), m_threadObject, SLOT(deleteLater()));
    connect(m_thread, SIGNAL(started()), m_threadObject, SLOT(loadWallpaperInfo()));

    m_thread->start();
}

QString Wallpaper::convertImgName(QString originName)
{
    //设置文件名,遍历文件夹中的文件
    QStringList imgPathList = originName.split("/", QString::SkipEmptyParts);
    QString dstName = imgPathList.join("_");
    return dstName;
}

QString Wallpaper::getDrawImgName(QString originName)
{
    QString drawImg;
    QFile file;
    file.setFileName(originName);
    if (!file.exists())
    {
        QString cacheImg = convertImgName(originName);
        file.setFileName(m_cacheDirName + cacheImg);
        if (file.exists())
            drawImg = m_cacheDirName + cacheImg;
        else
            return nullptr;
    }
    else
        drawImg = originName;
    return drawImg;
}
