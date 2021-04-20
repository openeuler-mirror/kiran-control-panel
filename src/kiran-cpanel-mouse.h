#ifndef KIRANCPANELMOUSE_H
#define KIRANCPANELMOUSE_H

#include <QWidget>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

#include "kiran-cpanel-mouse-widget.h"

class KiranCPanelMouse : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranCPanelMouse();
    ~KiranCPanelMouse();

private:
    KiranCPanelMouseWidget* m_cpanelMouseWidget;

};

#endif // KIRANCPANELMOUSE_H
