/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANCONTROLPANELMODULERUNALONEWINDOW_H
#define KIRANCONTROLPANELMODULERUNALONEWINDOW_H

#include "/usr/include/kiranwidgets-qt5/kiran-titlebar-window.h"
#include "kiran-control-panel-global.h"
#include <QWidget>
using namespace KiranControlPanelGlobal;

class KiranControlPanelModuleRunaloneWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranControlPanelModuleRunaloneWindow();
    ~KiranControlPanelModuleRunaloneWindow();
    void setModeName(const QString &name);

private:
     QMap<int, ModelItem> m_map;
};

#endif // KIRANCONTROLPANELMODULERUNALONEWINDOW_H
