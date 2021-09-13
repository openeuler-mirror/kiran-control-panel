#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QWidget>
enum
{
    KCP_LIST_ITEM_GENERAL,
    KCP_LIST_ITEM_LAYOUT
};

namespace Ui
{
class MainWindow;
}

class MainWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void addSidebarItem(QString text, QString icon);

private:
    Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
