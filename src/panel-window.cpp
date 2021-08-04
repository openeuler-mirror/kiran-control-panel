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

PanelWindow::PanelWindow(QWidget *parent) : KiranTitlebarWindow(parent)
{
    initUI();
}

void PanelWindow::initUI()
{
    //初始化窗口相关
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setIcon(QIcon::fromTheme(qAppName()));
    setTitle(tr("Control Panel"));

    //初始化中心显示窗口
    auto *centerWidget = new PanelWidget(this);
    setWindowContentWidget(centerWidget);

    //添加搜索框
    auto *searchBox = new CPanelSearchEdit(this);
    searchBox->setFixedSize(352, 40);
    getTitlebarCustomLayout()->addWidget(searchBox);
    setTitlebarCustomLayoutAlignHCenter(true);
}