#include "kiran-cpanel-mouse.h"
#include <QDesktopWidget>
#include <QApplication>
#include <iostream>

using namespace std;

KiranCPanelMouse::KiranCPanelMouse() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setTitle(tr("Mouse and TouchPad"));

    m_cpanelMouseWidget = new KiranCPanelMouseWidget(this);
    setWindowContentWidget(m_cpanelMouseWidget);
    connect(m_cpanelMouseWidget, &KiranCPanelMouse::destroyed, this, &KiranCPanelMouse::close);

}

KiranCPanelMouse::~KiranCPanelMouse()
{

}

QSize KiranCPanelMouse::sizeHint() const
{
    /*根据系统分辨率设置窗口大小*/
    QDesktopWidget *desktop = QApplication::desktop();
    cout << desktop->width() << desktop->height() <<endl;
    QSize windowSize;
    if(desktop->height() >= 730 && desktop->width() >= 918 ) //能显示全
    {
        windowSize = QSize(918,730);
    }
    else
    {
        windowSize = QSize(desktop->width() , desktop->height());
    }

    return windowSize;
}
