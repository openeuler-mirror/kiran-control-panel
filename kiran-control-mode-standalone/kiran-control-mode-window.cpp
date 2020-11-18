/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-control-mode-window.h"
#include "../kiran-mode-item-wgt.h"
#include <QIcon>
#include <QDebug>

KiranControlModeWindow::KiranControlModeWindow() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    resize(708, 830);
}

KiranControlModeWindow::~KiranControlModeWindow()
{

}

void KiranControlModeWindow::setModeName(const QString &name)
{
    ModelItem data = getModeItem(name);
    m_map.insert(1, data);

    setIcon(QIcon(data.icon));
    setTitle(data.name);
    KiranModeItemWgt *centerWgt = new KiranModeItemWgt(this);
    centerWgt->setData(m_map);
    setWindowContentWidget(centerWgt);
}
