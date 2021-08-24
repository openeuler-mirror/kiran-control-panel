#ifndef KCPWINDOW_H
#define KCPWINDOW_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QWidget>
enum
{
    KCP_LIST_ITEM_GENERAL,
    KCP_LIST_ITEM_LAYOUT
};

namespace Ui
{
class KCPWindow;
}

class KCPWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KCPWindow(QWidget *parent = 0);
    ~KCPWindow();

private:
    void addSidebarItem(QString text, QString icon);

private:
    Ui::KCPWindow *ui;
};

#endif  // KCPWINDOW_H
