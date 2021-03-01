//
// Created by lxh on 2021/3/1.
//

#include "kiran-time-date-window.h"
#include "kiran-timedate-widget.h"
#include <QIcon>

KiranTimeDateWindow::KiranTimeDateWindow(QWidget *parent)
    :KiranTitlebarWindow(){
    setWindowContentWidget(new KiranTimeDateWidget(this));
    setTitle(tr("Time And Date Manager"));
    QIcon icon = QIcon::fromTheme("preferences-system-time");
    setIcon(icon);
}

KiranTimeDateWindow::~KiranTimeDateWindow() {

}
