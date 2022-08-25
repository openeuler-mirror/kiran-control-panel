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
#include "search-edit/search-edit.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QX11Info>
#include <QJsonDocument>
#include <QJsonObject>
#include <qt5-log-i.h>

#include <kiran-single-application.h>

PanelWindow::PanelWindow(QWidget *parent) : KiranTitlebarWindow(parent)
{
    initUI();
    connect(dynamic_cast<KiranSingleApplication*>(qApp),&KiranSingleApplication::instanceStarted,this,&PanelWindow::handleInstanceStarted);
    connect(dynamic_cast<KiranSingleApplication*>(qApp),&KiranSingleApplication::receivedMessage,this,&PanelWindow::handleReceivedMessage);
}

void PanelWindow::initUI()
{
    //初始化窗口相关
    setTitleBarHeight(40);
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setIcon(QIcon::fromTheme("kiran-control-panel"));
    setTitle(tr("Control Panel"));

    //初始化中心显示窗口
    m_panelWidget = new PanelWidget(this);
    setWindowContentWidget(m_panelWidget);

    //添加搜索框
    auto *searchBox = new SearchEdit(this);
    searchBox->setAccessibleName("ControlPanelSearchEdit");
    searchBox->setFixedSize(352, 30);
    getTitlebarCustomLayout()->addWidget(searchBox);
    setTitlebarCustomLayoutAlignHCenter(true);

    connect(searchBox,&SearchEdit::requestJumpTo,this,&PanelWindow::jump);
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


void PanelWindow::handleReceivedMessage(quint32 instanceId, QByteArray message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonObject jsonObject = doc.object();
    QString category = jsonObject["category"].toString();
    QString subItem = jsonObject["subitem"].toString();
    jump(category,subItem);
}

void PanelWindow::jump(const QString& categoryName,const QString& subItem)
{
    KLOG_DEBUG() << "jump to" << categoryName << subItem;
    m_panelWidget->jumpTo(categoryName,subItem);
}

#include <QResizeEvent>
void PanelWindow::resizeEvent(QResizeEvent *event)
{
    KLOG_DEBUG() << "---> kiran control panel resize event:" << event->size();
    QWidget::resizeEvent(event);
}
