#include "kiran-date-time-widget.h"

#include "log.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTimeZone>

#define DEFAULT_STYLE_FILE ":/themes/black_theme.qss"

int main(int argc, char *argv[])
{
    ///初始化日志模块
    Log::instance()->init("/tmp/kiran-datetime-manager.log");
    qInstallMessageHandler(Log::messageHandler);

    QApplication a(argc, argv);

    QFile file(DEFAULT_STYLE_FILE);
    if(file.open(QIODevice::ReadOnly)){
        qApp->setStyleSheet(file.readAll()+qApp->styleSheet());
    }else{
        qWarning() << "load style sheet failed";
    }

    KiranDateTimeWidget widget;
    widget.show();
    return a.exec();
}
