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
    void createPreviewLabel();
    void createChooserWidget();

private:
    Ui::Wallpaper *ui;
    QString m_currDesktopWp;
    QString m_currLockScreenWp;


};

#endif // WALLPAPER_H
