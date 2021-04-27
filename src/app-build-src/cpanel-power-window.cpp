//
// Created by lxh on 2021/4/6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_kiran-power-manager-window.h" resolved

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
