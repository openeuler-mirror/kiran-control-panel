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
 * Author:     yangxiaoqing <yangxiaoqing@kylinsec.com.cn>
 */

#include "launcher.h"
#include "kiran-module-widget/kiran-module-widget.h"
#include "kiran-single-application.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QIcon>
#include <QScreen>
#include <QX11Info>


Launcher::Launcher(QWidget *parent) :
    KiranTitlebarWindow(parent)
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    m_moduleWidget = new KiranModuleWidget(this);

    setWindowContentWidget(m_moduleWidget);
    connect(static_cast<KiranSingleApplication*>(qApp),&KiranSingleApplication::instanceStarted,this,&Launcher::handleActivateSlot);
}

Launcher::~Launcher()
{

}

void Launcher::setSubItems(QVector<KcpPluginSubItemPtr> subitems)
{
    m_moduleWidget->setSubItems(subitems);
}

QSize Launcher::sizeHint() const
{
    return KiranTitlebarWindow::sizeHint();
}

void Launcher::handleActivateSlot()
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
