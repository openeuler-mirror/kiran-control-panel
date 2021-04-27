//
// Created by lxh on 2021/4/6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_kiran-power-manager-window.h" resolved

#include "kiran-power-manager-window.h"
#include "ui_kiran-power-manager-window.h"
#include "kiran-power-manager-widget.h"

KiranPowerManagerWindow::KiranPowerManagerWindow(QWidget *parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::KiranPowerManagerWindow)
{
    auto powerWidget = new KiranPowerManagerWidget(this);
    setWindowContentWidget(powerWidget);
    setTitle(tr("Power Manager"));
}

KiranPowerManagerWindow::~KiranPowerManagerWindow()
{
    delete ui;
}
