/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */


#include "panel-window.h"
#include "plugin-manager.h"
#include "panel-search-edit.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QX11Info>

#include <kiran-single-application.h>

PanelWindow::PanelWindow(QWidget *parent) : KiranTitlebarWindow(parent)
{
    initUI();
    connect(static_cast<KiranSingleApplication*>(qApp),&KiranSingleApplication::instanceStarted,this,&PanelWindow::handleInstanceStarted);
}

void PanelWindow::initUI()
{
    //初始化窗口相关
    setTitleBarHeight(40);
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setIcon(QIcon::fromTheme(qAppName()));
    setTitle(tr("Control Panel"));

    //初始化中心显示窗口
    auto *centerWidget = new PanelWidget(this);
    setWindowContentWidget(centerWidget);

    //添加搜索框
    auto *searchBox = new CPanelSearchEdit(this);
    searchBox->setFixedSize(352, 30);
    getTitlebarCustomLayout()->addWidget(searchBox);
    setTitlebarCustomLayoutAlignHCenter(true);
}

void PanelWindow::handleInstanceStarted()
{
    /*
     *由于QXcbWindow::requestActivateWindow
     *之中对root窗口发送_NET_ACTIVE_WINDOW的事件之中的时间戳未更新,
     *导致窗口管理器接收时事件戳较为落后，未被正确处理
     *暂时处理办法，手动更新下X11时间，避免事件戳落后
     */

    if( windowState() & Qt::WindowMinimized )
    {
        setWindowState(windowState()&~Qt::WindowMinimized);
    }

    QX11Info::setAppTime(QX11Info::getTimestamp());
    raise();
    activateWindow();
}
