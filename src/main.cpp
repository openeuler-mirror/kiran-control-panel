/**
* Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
* kiran-session-manager is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*
* Author:     yuanxing <yuanxing@kylinos.com.cn>
*/

#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-single-application.h>
#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <iostream>
#include "main-window.h"
#define TRANSLATION_DIR "/usr/share/kiran-cpanel-keyboard/translation/"

int main(int argc, char *argv[])
{
    //设置日志输出
    if (klog_qt5_init("", "kylinsec-session", "kiran-cpanel-keyboard", "kiran-cpanel-keyboard") < 0)
    {
        std::cout << "init klog error" << std::endl;
    }
    KLOG_INFO("******New Output*********\n");

    //只允许单个程序运行
    KiranSingleApplication a(argc, argv);

    QFile file(":/style/style.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        KiranMessageBox::message(nullptr, "warning", "Open failed", KiranMessageBox::Ok);
    }

    //加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if (qtTranslator->load(QLocale(), "kiran-cpanel-keyboard", ".", TRANSLATION_DIR, ".qm"))
    {
        a.installTranslator(qtTranslator);
    }
    else
    {
        qDebug("Load Translator File failed : %s\n", TRANSLATION_DIR);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
