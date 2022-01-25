/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */


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
#include <locale.h>

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

    KiranSingleApplication app(argc, argv,false,
                               KiranSingleApplication::Mode::User|KiranSingleApplication::Mode::SecondaryNotification);

    ///NOTE: 由于strftime获取系统locale进行格式化，Qt使用UTF8,若编码设置不为UTF8中文环境下会导致乱码
    ///所以LANG后面的编码若不为UTF-8,修改成UTF-8,使获取时间都为UTF-8格式
    QString lang = qgetenv("LANG");
    if(lang.contains("."))
    {
        QStringList splitRes = lang.split(".",QString::SkipEmptyParts);
        if(splitRes.size() == 2 && splitRes.at(1)!="UTF-8" )
        {
            splitRes.replace(1,"UTF-8");
            QString newLocale = splitRes.join(".");
            setlocale(LC_TIME,newLocale.toStdString().c_str());
        }
    }

    //为了保持插件使用启动器进行启动后，底部面板不堆叠，插件图标显示正常，
    //设置ApplicationName,更新窗口WM_CLASS属性为插件desktop名称
    if( !pluginDesktopName.isEmpty() )
    {
        QApplication::setApplicationName(pluginDesktopName);
    }

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
        KLOG_ERROR() << "can't load translator!" << qmFile;
    }

    QString pluginDesktopPath = QString("%1/%2").arg(PLUGIN_DESKTOP_DIR).arg(pluginDesktopName);
    if (!pluginDesktopPath.endsWith(".desktop"))
    {
        pluginDesktopPath.append(".desktop");
    }

    if (klog_qt5_init("", "kylinsec-session","kiran-cpanel-launcher", pluginDesktopName) != 0)
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
    KLOG_DEBUG() << "plugin info: "<< "\n"
                   << "\tdesktop  :" << pluginDesktopPath          << "\n"
                   << "\tName:    "  << pluginDesktopInfo.name     << "\n"
                   << "\tIcon:    "  << pluginDesktopInfo.Icon     << "\n"
                   << "\tComment: "  << pluginDesktopInfo.comment  << "\n"
                   << "\tCategory:"  << pluginDesktopInfo.category << "\n"
                   << "\tLibrary: "  << pluginDesktopInfo.library;

    KLOG_DEBUG() << "subItem info:";
    for (int i = 0; i < pluginDesktopInfo.subItems.size(); i++)
    {
        auto subItemInfo = pluginDesktopInfo.subItems.at(i);
        KLOG_DEBUG() << "item" << i << "\n"
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
    KLOG_DEBUG() << "sizeHint:" << w.sizeHint();
    w.resize(w.sizeHint());

    int screeNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screeNum);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);
    w.show();
    return KiranApplication::exec();
}
