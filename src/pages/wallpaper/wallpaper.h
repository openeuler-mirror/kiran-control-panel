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
    Ui::Wallpaper *ui;
};

#endif // WALLPAPER_H
