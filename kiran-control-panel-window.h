#ifndef KIRANCONTROLPANELWINDOW_H
#define KIRANCONTROLPANELWINDOW_H

#include <QLineEdit>
#include "/usr/include/kiranwidgets-qt5/kiran-titlebar-window.h"

class KiranControlPanelWindow : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit KiranControlPanelWindow();

signals:

public slots:

private:
    QLineEdit *m_search;
};

#endif // KIRANCONTROLPANELWINDOW_H
