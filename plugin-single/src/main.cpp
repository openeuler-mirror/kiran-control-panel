/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-control-center-single-plugin.h"
#include <QApplication>
#include <QCommandLineParser>

extern QString gLocaleName;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(QGuiApplication::translate("main", "kiran control panel module runalone"));  // 设置应用程序描述信息
    parser.addHelpOption();  // 添加帮助选项 （"-h" 或 "--help"）
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);  // 举例说明：将 "-adb" 当成一个选项来看，而不是看成 "-a -b -c"

    //    parser.addPositionalArgument("xxx", QGuiApplication::translate("main", "?????? undefined"));

    //    QCommandLineOption modeDesktopFilePath(QStringList() << "path" << "DesktopFilePath",
    //                                           QGuiApplication::translate("main", "Mode desktop file path."),
    //                                           QGuiApplication::translate("main", "DesktopFilePath"), "800");
    //    parser.addOption(modeDesktopFilePath);

    QCommandLineOption moduleDesktopFileName(QStringList() << "m-name" << "Module Name",
                                           QGuiApplication::translate("main", "Module name."),
                                           QGuiApplication::translate("main", "Module name."), "Fonts");
    parser.addOption(moduleDesktopFileName);
    parser.process(a);

    QString fileName = parser.value(moduleDesktopFileName);
    //
    gLocaleName = QLocale::system().name();//当前翻译类型,必须在加载模块数据之前赋值.模块的名称将根据此变量选择翻译类型.
    //
    KiranControlCenterSinglePlugin w;
    w.setModuleName(fileName);
    w.show();

    return a.exec();
}
