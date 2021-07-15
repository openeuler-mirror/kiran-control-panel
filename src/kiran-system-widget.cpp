/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
 */

#include "kiran-system-widget.h"
#include "kiran-system-information.h"
#include "license/user-license-agreement.h"
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <kiran-log/qt5-log-i.h>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QAction>
#include <QEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QStandardPaths>
#include <QTimer>

#define EULAFILE "/usr/share/kylin-release/EULA"

KiranSystemWidget::KiranSystemWidget():
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setTitle(tr("kiran-system-imformation"));
    setIcon(QIcon(":/images/kylin-about.png"));

    centerWgt = new kiranSystemInformation;
    setWindowContentWidget(centerWgt);
    connect(centerWgt, &kiranSystemInformation::destroyed, this, &KiranSystemWidget::close);
}

KiranSystemWidget::~KiranSystemWidget()
{
    if(centerWgt != nullptr)
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
    if(screenGeometry.height() >= 776 && screenGeometry.width() >= 980 ) //能显示全
    {
        windowSize = QSize(980,776);
    }
    else
    {
        windowSize = QSize(screenGeometry.width() , screenGeometry.height());
    }

    return windowSize;
}





