/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-control-panel-model-runalone-window.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(QGuiApplication::translate("main", "kiran contro center mode standalone"));  // 设置应用程序描述信息
    parser.addHelpOption();  // 添加帮助选项 （"-h" 或 "--help"）
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);  // 举例说明：将 "-adb" 当成一个选项来看，而不是看成 "-a -b -c"

    //    parser.addPositionalArgument("xxx", QGuiApplication::translate("main", "?????? undefined"));

    //    QCommandLineOption modeDesktopFilePath(QStringList() << "path" << "DesktopFilePath",
    //                                           QGuiApplication::translate("main", "Mode desktop file path."),
    //                                           QGuiApplication::translate("main", "DesktopFilePath"), "800");
    //    parser.addOption(modeDesktopFilePath);

    QCommandLineOption modeDesktopFileName(QStringList() << "name" << "DesktopFileName",
                                           QGuiApplication::translate("main", "Mode desktop file name."),
                                           QGuiApplication::translate("main", "DesktopFileName"), "Fonts");
    parser.addOption(modeDesktopFileName);
    parser.process(a);

    QString fileName = parser.value(modeDesktopFileName) ;

    KiranControlPanelModelRunaloneWindow w;
    w.setModeName(fileName);
    w.show();

    return a.exec();
}
