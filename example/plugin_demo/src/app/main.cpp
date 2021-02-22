/**
 * @file main.cpp
 * @brief description
 * @author yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
*/
#include "plugin-demo-window.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QTranslator>
#include <QPushButton>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString locale = QLocale::system().name();
    QString qmFile = QString("%1.%2.qm").arg(APP_TRANSLATE_PATH_PREFIX).arg(locale);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        a.installTranslator(&translator);

    PluginDemoWindow w;
    QDesktopWidget* desktopWidget = QApplication::desktop();
    const QRect screenRect = desktopWidget->screenGeometry();
    w.move(screenRect.center()-QPoint(w.rect().width()/2, w.rect().height()/2));
    w.show();

    return a.exec();
}
