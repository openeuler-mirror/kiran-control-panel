#ifndef KIRANCPANELAPPEARANCE_H
#define KIRANCPANELAPPEARANCE_H
#include <kiran-titlebar-window.h>

#include <QWidget>

namespace Ui {
class KiranCpanelAppearance;
}

enum pages{
    PAGE_BACKGROUND = 0,
    PAGE_THEME,
    PAGE_FONT,
    PAGE_BACKGROND_SELECT,
    PAGE_ICON_THEME,
    PAGE_CURSOR_THEME
};

enum items{
    ITEM_BACKGROUND = 0,
    ITEM_THEME,
    ITEM_FONT
};

class QListWidgetItem;
class KiranCpanelAppearance : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranCpanelAppearance(QWidget *parent = 0);
    ~KiranCpanelAppearance();

private:
    bool initUI();
    void *addSidebarItem(QString,QString);

private:
    Ui::KiranCpanelAppearance *ui;
};

#endif // KIRANCPANELAPPEARANCE_H
