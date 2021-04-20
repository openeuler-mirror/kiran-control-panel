#include "kiran-cpanel-mouse.h"

KiranCPanelMouse::KiranCPanelMouse() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setTitle(tr("Mouse and TouchPad"));

    m_cpanelMouseWidget = new KiranCPanelMouseWidget(this);

}

KiranCPanelMouse::~KiranCPanelMouse()
{

}
