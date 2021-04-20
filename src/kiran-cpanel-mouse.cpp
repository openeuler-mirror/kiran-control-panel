#include "kiran-cpanel-mouse.h"

KiranCPanelMouse::KiranCPanelMouse() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setTitle(tr("Mouse and TouchPad"));
}

KiranCPanelMouse::~KiranCPanelMouse()
{

}
