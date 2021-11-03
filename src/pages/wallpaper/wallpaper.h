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

#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QMap>
#include <QWidget>
namespace Ui
{
class Wallpaper;
}

class ImageSelector;
class ThreadObject;
class ChooserWidget;
class Wallpaper : public QWidget
{
    Q_OBJECT

public:
    explicit Wallpaper(QWidget *parent = 0);
    ~Wallpaper();

    virtual QSize sizeHint() const override;

private:
    void initUI();
    void createPreviewLabel();
    void createChooserWidget();
    void handleImageSelector();
    void loadVisibleWallpapers();
    QString convertImgName(QString originName);

public slots:
    void handleWallpaperInfo(QList<QMap<QString, QString>> wallpaperMapList);

signals:
    //void updateWallpaper(QList<QMap<QString,QString>> wallpaperMapList);
    void wallpaperChanged(int type, QString path);

private:
    Ui::Wallpaper *ui;
    QString m_currDesktopWp;
    QString m_currLockScreenWp;
    ChooserWidget *m_desktopWpChooser;
    ChooserWidget *m_lockScreenWPChooser;

    ImageSelector *m_imageSelector;
    QStringList m_customWallpaper;
    QThread *m_thread;
    ThreadObject *m_threadObject;
    QList<QMap<QString, QString>> m_wallpaperMapList;
};

#endif  // WALLPAPER_H
