/**
 * @file kiran-cpanel-mouse.cpp
 * @brief  用于加载kiran标题栏的主界面
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#include "kiran-cpanel-mouse.h"
#include <kiran-log/qt5-log-i.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>

using namespace std;

KiranCPanelMouse::KiranCPanelMouse() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setTitle(tr("Mouse and TouchPad"));
    setIcon(QIcon(":/images/kiran-cpanel-mouse.png"));

    m_cpanelMouseWidget = new KiranCPanelMouseWidget(this);
    setWindowContentWidget(m_cpanelMouseWidget);
}

KiranCPanelMouse::~KiranCPanelMouse()
{

}

/**
 * @brief 初始化主窗口大小
 * @return 根据屏幕分辨率返回初始大小
 */
QSize KiranCPanelMouse::sizeHint() const
{
    /*根据系统分辨率设置窗口大小*/
    QDesktopWidget *desktop = QApplication::desktop();
    KLOG_INFO() << "desktop width = " << desktop->width() <<
               " desktop height = " << desktop->height() << endl;
    QSize windowSize;
    if(desktop->height() >= 750 && desktop->width() >= 918 ) //能显示全
    {
        windowSize = QSize(918,750);
    }
    else
    {
        windowSize = QSize(desktop->width() , desktop->height());
    }

    return windowSize;
}
