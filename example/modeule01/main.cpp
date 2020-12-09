/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "dialog001.h"
#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QDebug>

extern QString gLocaleName;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString qmFile = "/root/kiran-control-panel/example/modeule01/module01.zh_CN.qm";
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        qApp->installTranslator(&translator);

    //窗口屏幕居中显示.
    Dialog001 w;
    const QRect &screenRect = QApplication::desktop()->screenGeometry();
    w.move(screenRect.center()-QPoint(w.rect().width()/2, w.rect().height()/2));
    w.show();
    return a.exec();
}
