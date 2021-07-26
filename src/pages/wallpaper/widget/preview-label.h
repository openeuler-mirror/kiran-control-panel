#ifndef PREVIEWLABEL_H
#define PREVIEWLABEL_H

#include <QWidget>
#include <QLabel>

class PreviewLabel : public QLabel
{
public:
    explicit PreviewLabel(int wallpaperType, QString imgPath, QWidget *parent = 0);
    void setWallpaperPath(QString path);
    void setWallpaperType(int type);
    void updateWallpaper(int type,QString path);

private:
    void drawDesktopPreview(QPainter *painter);
    void drawLockScreenPreview(QPainter *painter);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage  m_wallpaperImg;
    int m_wallpaperType;
};

#endif // PREVIEWLABEL_H
