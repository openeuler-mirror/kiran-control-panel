#include <kiran-single-application.h>
#include <qt5-log-i.h>
#include <QCommandLineParser>
#include <QDesktopWidget>
#include <QFile>
#include <QLoggingCategory>
#include <QTranslator>
#include <iostream>

#include "config.h"
#include "panel-window.h"
#include "plugin-manager.h"

using namespace std;

bool installTranslator()
{
    QTranslator tsor;
    if (!tsor.load(QLocale(),
                   qAppName() /*filename*/,
                   "." /*prefix*/,
                   TRANSLATE_PREFIX /*dir*/,
                   ".qm" /*suffix*/))
    {
        KLOG_ERROR_S() << "load translator failed!";
        return false;
    }
    QCoreApplication::installTranslator(&tsor);
    return true;
}

bool loadStyleSheet()
{
    QFile file(":/kiran-control-panel/themes/kiran-black.qss");
    if (file.open(QIODevice::ReadOnly))
    {
        QString styleContent = file.readAll();
        qApp->setStyleSheet(styleContent);
        return true;
    }
    KLOG_ERROR_S() << "can't load style sheet!";
    return false;
}

int main(int argc, char *argv[])
{
    KiranApplication a(argc, argv);

    //默认开启QtDebug输出，交由zlog进行过滤
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

    //初始化日志库
    klog_qt5_init("",
                  "kylinsec-session",
                  "kiran-control-panel",
                  "kiran-control-panel");

    //加载相关插件的信息
    CPanelPluginManager::getInstance()->loadAll();

    //安装翻译
    installTranslator();

    //加载样式表
    loadStyleSheet();

    PanelWindow w;
    int screeNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screeNum);
    w.resize(1060, 830);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);
    w.show();
    w.setContentWrapperMarginBottom(0);

    int execRet = QApplication::exec();

    ///销毁插件相关信息
    CPanelPluginManager::getInstance()->deleteInstance();

    return execRet;
}
