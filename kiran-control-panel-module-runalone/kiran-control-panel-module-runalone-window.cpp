/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-control-panel-module-runalone-window.h"
#include "kiran-module-widget/kiran-module-widget.h"
#include <QIcon>
#include <QDebug>

KiranControlPanelModuleRunaloneWindow::KiranControlPanelModuleRunaloneWindow() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    resize(708, 830);
}

KiranControlPanelModuleRunaloneWindow::~KiranControlPanelModuleRunaloneWindow()
{

}

void KiranControlPanelModuleRunaloneWindow::setModeName(const QString &name)
{
    ModelItem data = getModeItem(name);
    m_map.insert(1, data);

    setIcon(QIcon(data.icon));
    setTitle(data.name);
    KiranModuleWidget *centerWgt = new KiranModuleWidget(this);
    centerWgt->setModelsData(m_map);
    setWindowContentWidget(centerWgt);
}
