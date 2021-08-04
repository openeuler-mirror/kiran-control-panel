#include "kiran-cpanel-appearance.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QDebug>
#include <iostream>
#include <QTranslator>
#include <QDesktopWidget>
#include <kiran-single-application.h>
#include <kiran-message-box.h>
#include <QFile>
#include <kiran-log/qt5-log-i.h>
#include "config/config.h"

//TODO:use config.h
#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

int main(int argc, char *argv[])
{
    //设置日志输出
    QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtDebugMsg,true);
    if (klog_qt5_init("", "kylinsec-session", "kiran-cpanel-appearance", "kiran-cpanel-appearance") < 0){
        std::cout << "init klog error" << std::endl;
    }

    KLOG_INFO() <<"******New Output*********";

    KiranSingleApplication a(argc, argv);
    
    //加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if(qtTranslator->load(QLocale(),"kiran-cpanel-appearance",".",TRANSLATION_DIR,".qm")){
        a.installTranslator(qtTranslator);
    }
    else
    {
        KLOG_DEBUG("Load Translator File failed : %s\n", TRANSLATION_DIR);
    }

    QFile file(":/themes/style.qss");
    if( file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        KiranMessageBox::message(nullptr, "warning", "Open failed", KiranMessageBox::Ok);
    }

    KiranCpanelAppearance w;
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    w.move(screenGeometry.x()+(screenGeometry.width()-w.width())/2,
           screenGeometry.y()+(screenGeometry.height()-w.height())/2);

    w.resize(980,760);
    w.show();

    return a.exec();
}
