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

#include <kiran-single-application.h>
#include <qt5-log-i.h>
#include <iostream>
#include <QCommandLineParser>
#include <QDesktopWidget>
#include <QLoggingCategory>
#include <QTranslator>
#include <QJsonObject>
#include <QJsonDocument>

#include "config.h"
#include "panel-window.h"
#include "plugin-manager.h"

using namespace std;

bool installTranslator()
{
    QTranslator tsor;
    if (!tsor.load(QLocale(),
                   qAppName(),
                   ".",
                   TRANSLATE_PREFIX,
                   ".qm"))
    {
        KLOG_ERROR() << "kiran-control-panel load translator failed!";
        return false;
    }

    QCoreApplication::installTranslator(&tsor);
    return true;
}

QString defaultCategory = "about-system";
QString defaultSubItem = "SystemInformation";

void processCommandLine()
{
    KiranSingleApplication* singleApp = dynamic_cast<KiranSingleApplication*>(qApp);

    QCommandLineParser cmdParser;
    QCommandLineOption categoryOption("c","主面板进入哪个分类","category");
    QCommandLineOption subItemOption("s","分类下的哪个子项","subItem");
    cmdParser.addHelpOption();
    cmdParser.addOptions({categoryOption,subItemOption});
    cmdParser.process(*singleApp);

    QString category = cmdParser.value(categoryOption);
    QString subItem = cmdParser.value(categoryOption);
    if( !subItem.isEmpty() && category.isEmpty() )
    {
        std::cerr << "failed to set sub item without category" << std::endl;
        exit(EXIT_FAILURE);
    }
    if( !category.isEmpty() )
    {
        if( !singleApp->isPrimary() )
        {
            QJsonObject jumpInfoObject({{"category", category},{"subitem",subItem}});
            QJsonDocument jumpInfoDoc(jumpInfoObject);
            QByteArray byteArray = jumpInfoDoc.toJson();
        }
        else
        {
            defaultCategory = category;
            defaultSubItem = subItem;
        }
    }
}

int main(int argc, char *argv[])
{
    KiranSingleApplication app(argc,
                             argv,
                             true,
                             KiranSingleApplication::Mode::User | KiranSingleApplication::Mode::SecondaryNotification);

    // 初始化日志库
    klog_qt5_init("","kylinsec-session","kiran-control-panel","kiran-control-panel");

    // 处理命令行参数
    processCommandLine();

    // 加载相关插件的信息
    PluginManager::getInstance()->loadAll();

    // 安装翻译
    installTranslator();

    PanelWindow w;
    int screeNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screeNum);
    w.resize(1060, 830);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);
    w.show();
    w.setContentWrapperMarginBottom(0);

    int execRet = app.exec();

    // 销毁插件相关信息
    PluginManager::getInstance()->deleteInstance();

    return execRet;
}
