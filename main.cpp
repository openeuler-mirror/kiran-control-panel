#include "kiran-timedate-widget.h"
#include "log.h"
#include "kiran-timedate-global-data.h"
#include "SingleApplication/singleapplication.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QGSettings>
#include <QLocale>

#define DEFAULT_STYLE_FILE ":/themes/black_theme.qss"
#define TRANSLATION_FILE_DIR "/usr/share/kiran-timedate-manager/translations/"

#define MATE_INTERFACE_SCHEMA "org.mate.interface"
#define KEY_SCALING_FACTOR  "window-scaling-factor"

void loadStyleSheet()
{
    QFile file(DEFAULT_STYLE_FILE);
    if(file.open(QIODevice::ReadOnly)){
        qApp->setStyleSheet(file.readAll()+qApp->styleSheet());
    }else{
        qWarning() << "load style sheet failed";
    }
}

///TODO:样式表清理
int main(int argc, char *argv[])
{
    ///安装日志输出
    Log::instance()->init("/tmp/kiran-datetime-manager.log");
    qInstallMessageHandler(Log::messageHandler);
#ifdef TEST
    Log::instance()->setAppend2File(true);
    Log::instance()->setLogLevel(QtDebugMsg);
#endif

    SingleApplication app(argc, argv);

    ///翻译
    QTranslator tsor;
    //filename+prefix+language name+suffix
    qInfo() << "load translation file: " << tsor.load(QLocale(),
                                                      "kiran-timedate-manager"/*filename*/,
                                                      "."/*prefix*/,
                                                      TRANSLATION_FILE_DIR/*dir*/,
                                                      ".qm"/*suffix*/);
    qApp->installTranslator(&tsor);

    ///命令行解析
    QCommandLineOption logOutputOption({"g","debug"},QObject::tr("append log to file"));
    QCommandLineOption logLevelOption({"l","debug-level"},QObject::tr("debug log level(0/DebugMsg,1/WarnngMsg,2/CriticalMsg|SystemMsg,3/FatalMsg,4/InfoMsg)"),"level","0");
    QCommandLineParser cmdParser;

    cmdParser.addHelpOption();
    cmdParser.addOptions({logOutputOption,logLevelOption});
    cmdParser.process(app);

    if(cmdParser.isSet(logOutputOption)){
        Log::instance()->setAppend2File(true);
    }

    if(cmdParser.isSet(logLevelOption)){
        QString str = cmdParser.value(logLevelOption);
        bool ok = false;
        int loglevel = str.toInt(&ok);
        if( !ok||loglevel<0||loglevel>4 ){
            qWarning("invalid debug-level arguments");
            return -1;
        }
        QtMsgType msgType = static_cast<QtMsgType>(loglevel);
        Log::instance()->setLogLevel(msgType);
    }

    ///加载样式表
    loadStyleSheet();

    ///加载数据
    if( !KiranTimeDateGlobalData::instance()->init() ){
        QMessageBox::warning(nullptr,"",QObject::tr("Failed to load time zone information"),QString("ok"));
        return -1;
    }

    KiranTimeDateWidget widget;
    widget.show();

    return app.exec();
}
