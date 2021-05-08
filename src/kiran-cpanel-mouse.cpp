/**
 * @file kiran-cpanel-mouse.cpp
 * @brief  用于加载kiran标题栏的主界面
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

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
    if(desktop->height() >= 854 && desktop->width() >= 918 ) //能显示全
    {
        windowSize = QSize(956,760);
    }
    else
    {
        windowSize = QSize(desktop->width() , desktop->height());
    }

    return windowSize;
}
