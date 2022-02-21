/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "kiran-system-widget.h"
#include <kiran-log/qt5-log-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextDocument>
#include <QTimer>
#include <QtPrintSupport/QPrinter>
#include "kiran-system-information.h"

#ifdef DISABLE_KIRANWIDGETS
KiranSystemWidget::KiranSystemWidget() : QWidget(), centerWgt(nullptr)
{
    setWindowFlag(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
#else
KiranSystemWidget::KiranSystemWidget() : KiranTitlebarWindow(), centerWgt(nullptr)
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    centerWgt = new kiranSystemInformation(this);
    setWindowContentWidget(centerWgt);
    connect(centerWgt, &kiranSystemInformation::destroyed, this, &KiranSystemWidget::close);
#endif

    setTitle(tr("kiran-system-imformation"));
    setIcon(QIcon(":/images/kylin-about.png"));
}

KiranSystemWidget::~KiranSystemWidget()
{
    if (centerWgt != nullptr)
    {
        delete centerWgt;
    }
}

QSize KiranSystemWidget::sizeHint() const
{
    /*根据系统分辨率设置窗口大小*/
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);

    KLOG_INFO() << screenGeometry.width() << screenGeometry.height();
    QSize windowSize;
    if (screenGeometry.height() >= 776 && screenGeometry.width() >= 980)  //能显示全
    {
        windowSize = QSize(980, 776);
    }
    else
    {
        windowSize = QSize(screenGeometry.width(), screenGeometry.height());
    }

    return windowSize;
}
