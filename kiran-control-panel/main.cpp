#include "kiran-control-panel-window.h"
#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString locale = QLocale::system().name();
    QString qmFile = QString("/usr/share/kiran-control-panel/kiran-control-panel.%1.qm").arg(locale);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        a.installTranslator(&translator);

    KiranControlPanelWindow w;
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    w.move(screenRect.center()-QPoint(w.rect().width()/2, w.rect().height()/2));
    w.show();
    return a.exec();
}
