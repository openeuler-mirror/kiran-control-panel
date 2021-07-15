/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
 */
#include "kiran-system-widget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QMessageBox>
#include <kiran-log/qt5-log-i.h>
#include <kiranwidgets-qt5/kiran-single-application.h>
#include <QLoggingCategory>
#include <QTranslator>
#include "config/config.h"
#include <iostream>
#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

int main(int argc, char *argv[])
{
    ///注册自定义的消息处理函数
    QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtDebugMsg,true);
    if (klog_qt5_init("", "kylinsec-system", "kiran-cpanel-system", "kiran-cpanel-system") < 0){
        std::cout << "init zlog error" << std::endl;
    }

    KLOG_INFO() <<"******New Output*********";

    KiranSingleApplication a(argc, argv);

    ///加载qss样式表
    QFile file(":/qss/style.qss");
    if( file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        QMessageBox::warning(NULL, "warning", "Open failed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }

    ///加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if(qtTranslator->load(QLocale(),"kiran-cpanel-system",".",TRANSLATION_DIR,".qm")){
        a.installTranslator(qtTranslator);
    }
    else
    {
        KLOG_DEBUG() << "Load Translator File failed : " << TRANSLATION_DIR;
    }

    KiranSystemWidget w;
    w.resize(w.sizeHint());

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    w.move(screenGeometry.x()+(screenGeometry.width()-w.width())/2,
           screenGeometry.y()+(screenGeometry.height()-w.height())/2);

    w.show();
    return a.exec();
}
