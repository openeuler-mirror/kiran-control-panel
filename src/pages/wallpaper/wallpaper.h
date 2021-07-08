#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QWidget>

namespace Ui {
class Wallpaper;
}

class Wallpaper : public QWidget
{
    Q_OBJECT

public:
    explicit Wallpaper(QWidget *parent = 0);
    ~Wallpaper();

private:
    void initUI();

private:
    Ui::Wallpaper *ui;
    QString m_currDesktopBgPath;
    QString m_currLockScreenBgPath;

};

#endif // WALLPAPER_H
