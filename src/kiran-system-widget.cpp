/**
 * @file kiran-system-widget.cpp
 * @brief 带有自定义标题栏的系统信息界面
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
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





