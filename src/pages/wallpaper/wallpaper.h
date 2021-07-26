#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QWidget>
#include <QMap>
namespace Ui {
class Wallpaper;
}

class ImageSelector;
class ThreadObject;
class Wallpaper : public QWidget
{
    Q_OBJECT

public:
    explicit Wallpaper(QWidget *parent = 0);
    ~Wallpaper();

private:
    void initUI();
    void createPreviewLabel();
    void createChooserWidget();
    void createImageSelector();
    void loadVisibleWallpapers();

public slots:
    void handleWallpaperInfo(QList<QMap<QString,QString>> wallpaperMapList);

signals:
    void updateWallpaper(QList<QMap<QString,QString>> wallpaperMapList);

private:
    Ui::Wallpaper *ui;
    QString m_currDesktopWp;
    QString m_currLockScreenWp;

    ImageSelector *m_imageSelector;
    QStringList m_visibleWallpaper;
    QThread *m_thread;
    ThreadObject *m_threadObject;
    QList<QMap<QString,QString>> m_wallpaperMapList;


};

#endif // WALLPAPER_H
