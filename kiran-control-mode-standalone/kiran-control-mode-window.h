/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANCONTROLMODEWINDOW_H
#define KIRANCONTROLMODEWINDOW_H

#include "/usr/include/kiranwidgets-qt5/kiran-titlebar-window.h"
#include "../kiran-control-panel-global.h"
#include <QWidget>
using namespace KiranControlPanelGlobal;

class KiranControlModeWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranControlModeWindow();
    ~KiranControlModeWindow();
    void setModeName(const QString &name);

private:
     QMap<int, ModelItem> m_map;
};

#endif // KIRANCONTROLMODEWINDOW_H
