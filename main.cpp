#include "kiran-date-time-widget.h"
#include "log.h"
#include "kiran-timedate-global-data.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

#define DEFAULT_STYLE_FILE ":/themes/black_theme.qss"

void installMessageHandler()
{
    Log::instance()->init("/tmp/kiran-datetime-manager.log");
    qInstallMessageHandler(Log::messageHandler);
}

void loadStyleSheet()
{
    QFile file(DEFAULT_STYLE_FILE);
    if(file.open(QIODevice::ReadOnly)){
        qApp->setStyleSheet(file.readAll()+qApp->styleSheet());
    }else{
        qWarning() << "load style sheet failed";
    }
}

int main(int argc, char *argv[])
{
    installMessageHandler();

    QApplication app(argc, argv);

    loadStyleSheet();

    if( !KiranTimeDateGlobalData::instance()->init() ){
        QMessageBox::warning(nullptr,"",QObject::tr("Failed to load time zone information"),QString("ok"));
    }

    KiranDateTimeWidget widget;
    widget.show();

    return app.exec();
}
