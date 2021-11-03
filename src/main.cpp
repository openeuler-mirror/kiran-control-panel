/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
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
#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QLoggingCategory>
#include <QTranslator>
#include <iostream>
#include "config/config.h"
#include "kiran-cpanel-appearance.h"

//TODO:use config.h
#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

int main(int argc, char *argv[])
{
    //设置日志输出
    QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtDebugMsg, true);
    if (klog_qt5_init("", "kylinsec-session", "kiran-cpanel-appearance", "kiran-cpanel-appearance") < 0)
    {
        std::cout << "init klog error" << std::endl;
    }

    KLOG_INFO() << "******New Output*********";

    KiranSingleApplication a(argc, argv);

    //加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if (qtTranslator->load(QLocale(), "kiran-cpanel-appearance", ".", TRANSLATION_DIR, ".qm"))
    {
        a.installTranslator(qtTranslator);
    }
    else
    {
        KLOG_DEBUG("Load Translator File failed : %s\n", TRANSLATION_DIR);
    }

    QFile file(":/themes/style.qss");
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

    KiranCpanelAppearance w;
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);

    w.resize(980, 760);
    w.show();

    return a.exec();
}
