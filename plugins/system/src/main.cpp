/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
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
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QLoggingCategory>
#include <QMessageBox>
#include <QTranslator>
#include <iostream>
#include "config/config.h"
#include "kiran-system-information.h"
#include "pages/license-information/license-information.h"

#ifndef DISABLE_KIRANWIDGETS
#include <kiran-single-application.h>
#endif

#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

int main(int argc, char *argv[])
{
#ifdef DISABLE_KIRANWIDGETS
    QApplication a(argc, argv);
#else
    KiranSingleApplication a(argc, argv);
#endif

    ///注册自定义的消息处理函数
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
    if (klog_qt5_init("", "kylinsec-session", "kiran-cpanel-system", "kiran-cpanel-system") < 0)
    {
        std::cout << "init zlog error" << std::endl;
    }

    KLOG_DEBUG() << "******New Output*********";

    ///加载qss样式表
    QFile file(":/qss/style.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        QMessageBox::warning(NULL, QObject::tr("warning"), QObject::tr("Open qss file failed"), QMessageBox::Ok);
    }

    ///加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if (qtTranslator->load(QLocale(), "kiran-cpanel-system", ".", TRANSLATION_DIR, ".qm"))
    {
        a.installTranslator(qtTranslator);
    }
    else
    {
        KLOG_DEBUG() << "Load Translator File failed : " << TRANSLATION_DIR;
    }

    kiranSystemInformation w;
    w.resize(w.sizeHint());

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);

    w.show();
    return a.exec();
}
