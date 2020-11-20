/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-control-panel-model-runalone-window.h"
#include "kiran-model-widget/kiran-model-widget.h"
#include <QIcon>
#include <QDebug>

KiranControlPanelModelRunaloneWindow::KiranControlPanelModelRunaloneWindow() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    resize(708, 830);
}

KiranControlPanelModelRunaloneWindow::~KiranControlPanelModelRunaloneWindow()
{

}

void KiranControlPanelModelRunaloneWindow::setModeName(const QString &name)
{
    ModelItem data = getModeItem(name);
    m_map.insert(1, data);

    setIcon(QIcon(data.icon));
    setTitle(data.name);
    KiranModelWidget *centerWgt = new KiranModelWidget(this);
    centerWgt->setModelsData(m_map);
    setWindowContentWidget(centerWgt);
}
