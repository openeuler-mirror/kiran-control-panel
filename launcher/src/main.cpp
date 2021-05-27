/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#include "launcher.h"
#include "plugin-info.h"

#include <kiran-single-application.h>
#include <qt5-log-i.h>
#include <QCommandLineParser>
#include <QDesktopWidget>
#include <QEvent>
#include <QIcon>
#include <QLayout>
#include <QLoggingCategory>
#include <QTranslator>
#include <iostream>

int main(int argc, char *argv[])
{
    ///先将插件选项从参数中提取出来,作为校验进程单例的一部分
    QStringList arguments;
    for (int i = 0; i < argc; i++)
    {
        arguments << argv[i];
    }
    QString pluginDesktopName;
    QCommandLineOption pluginOption("cpanel-plugin", "plugin library path or name list", "plugin", "");
    QCommandLineParser parser;
    parser.setApplicationDescription("kiran control panel module runalone");
    parser.addOption(pluginOption);
    QCommandLineOption helpOption = parser.addHelpOption();
    parser.parse(arguments);
    if (parser.isSet(pluginOption))
    {
        pluginDesktopName = parser.value(pluginOption);
        KiranSingleApplication::addApplicationIDUserData(pluginDesktopName);
    }

    KiranSingleApplication app(argc, argv);
    ///再次解析命令行参数是为了处理--help选项得到正确的输出
    parser.addHelpOption();
    parser.process(app);

    QTranslator translator;
    QLocale locale;
    if (translator.load(locale,
                        qAppName(),
                        ".",
                        TRANSLATE_PREFIX,
                        ".qm"))
    {
        qApp->installTranslator(&translator);
    }
    else
    {
        QString qmFile = QString("%1/%2%3%4%5").arg(TRANSLATE_PREFIX).arg(qAppName()).arg(".").arg(locale.name()).arg(".qm");
        KLOG_ERROR_S() << "can't load translator!" << qmFile;
    }

    QString pluginDesktopPath = QString("%1/%2").arg(PLUGIN_DESKTOP_DIR).arg(pluginDesktopName);
    if (!pluginDesktopPath.endsWith(".desktop"))
    {
        pluginDesktopPath.append(".desktop");
    }

    if (klog_qt5_init("", "kylinsec-session", qAppName(), pluginDesktopName) != 0)
    {
        KLOG_CERR("kiran log init error");
    }

    auto pluginHelper = CPanelPluginHelper::loadPlugin(pluginDesktopPath);
    if (!pluginHelper)
    {
        exit(EXIT_FAILURE);
    }

    //输出调试信息
    auto pluginDesktopInfo = pluginHelper->getPluginDesktopInfo();
    KLOG_DEBUG_S() << "plugin info: "
                   << "\n"
                   << "\tdesktop  :" << pluginDesktopPath << "\n"
                   << "\tName:    " << pluginDesktopInfo.name << "\n"
                   << "\tIcon:    " << pluginDesktopInfo.Icon << "\n"
                   << "\tComment: " << pluginDesktopInfo.comment << "\n"
                   << "\tCategory:" << pluginDesktopInfo.category << "\n"
                   << "\tLibrary: " << pluginDesktopInfo.library;

    KLOG_DEBUG_S() << "subItem info:";
    for (int i = 0; i < pluginDesktopInfo.subItems.size(); i++)
    {
        auto subItemInfo = pluginDesktopInfo.subItems.at(i);
        KLOG_DEBUG_S() << "item" << i << "\n"
                       << "\tID:      " << subItemInfo.id << "\n"
                       << "\tName:    " << subItemInfo.name << "\n"
                       << "\tIcon:    " << subItemInfo.icon << "\n"
                       << "\tKeywords:" << subItemInfo.keywords;
    }

    Launcher w;
    w.setTitle(pluginHelper->getPluginDesktopInfo().name);
    QIcon titleIcon = QIcon::fromTheme(pluginDesktopInfo.Icon);
    w.setIcon(titleIcon);
    w.setPlugin(pluginHelper);
    w.resize(w.sizeHint());

    int screeNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screeNum);
    w.resize(1060, 830);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);
    w.show();

    return KiranApplication::exec();
}
