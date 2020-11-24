#include "kiran-control-panel-window.h"
#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QDebug>

extern QString gLocaleName;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString locale = QLocale::system().name();
    gLocaleName = locale;//当前翻译类型,必须在加载模块数据之前赋值.模块的名称将根据此变量选择翻译类型.
    QString qmFile = QString("/usr/share/kiran-control-panel/kiran-control-panel.%1.qm").arg(locale);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        a.installTranslator(&translator);

    //窗口屏幕居中显示.
    KiranControlPanelWindow w;
    const QRect &screenRect = QApplication::desktop()->screenGeometry();
    w.move(screenRect.center()-QPoint(w.rect().width()/2, w.rect().height()/2));
    w.show();
    return a.exec();
}
