#include "kiran-system-widget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QLoggingCategory>
#include <QTranslator>
#include <single-application/single-application.h>
#include "general-functions-class.h"
#include "config/config.h"

#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

int main(int argc, char *argv[])
{
    ///注册自定义的消息处理函数
    qInstallMessageHandler(GeneralFunctionsClass::customMessageHandler);
    QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtDebugMsg,true);
    qDebug("******New Output*********\n");

    KiranSingleApplication a(argc, argv);

    qInfo()<< "one process " << endl;
    ///加载qss样式表
    QFile file(":/qss/style.qss");
    if( file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        QMessageBox::warning(NULL, "warning", "Open failed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }

    ///加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if(qtTranslator->load(QLocale(),"kiran-system-information",".",TRANSLATION_DIR,".qm")){
        a.installTranslator(qtTranslator);
    }
    else
    {
        qDebug("Load Translator File failed : %s\n", TRANSLATION_DIR);
    }

    KiranSystemWidget w;
    w.resize(w.sizeHint());
    w.show();
    QDesktopWidget *desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2 , (desktop->height() - w.height())/2 );
    return a.exec();
}
