/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-cpanel-power is licensed under Mulan PSL v2.
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

#include "cpanel-power-window.h"
#include "cpanel-power-widget.h"
#include "ui_cpanel-power-window.h"

CPanelPowerWindow::CPanelPowerWindow(QWidget *parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::CPanelPowerWindow)
{
    auto powerWidget = new CPanelPowerWidget(this);
    setWindowContentWidget(powerWidget);
    setTitle(tr("Power Manager"));
}

CPanelPowerWindow::~CPanelPowerWindow()
{
    delete ui;
}
