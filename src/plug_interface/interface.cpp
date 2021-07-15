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
#include "interface.h"
#include "pages/hardware-information/hardware-information-widget.h"
#include "pages/system-information/system-information-widget.h"
#include "system-info-dbus.h"
#include "config/config.h"
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <kiran-log/qt5-log-i.h>
#include <QLocale>
#include <QTranslator>
#include <QCoreApplication>
#include <QFile>
#include <iostream>
#include <QMessageBox>
#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

KcpInterface::KcpInterface()
{
}

bool KcpInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpInterface::visibleSubItems()
{
    QStringList subItems;
    subItems << "SystemInformationWidget" << "HardwareInformationWidget" ;
    return subItems;
}

QWidget *KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if (id == "SystemInformationWidget")
    {
        widget = new SystemInformationWidget();
    }
    else if (id == "HardwareInformationWidget")
    {
        widget = new HardwareInformationWidget();
    }
    m_currentWidget = widget;

    ///加载qss样式表
    QFile file(":/qss/style.qss");
    if( file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        m_currentWidget->setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        QMessageBox::warning(NULL, "warning", "load qss file failed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    return m_currentWidget;
}

void KcpInterface::uninit()
{
    if( m_translator )
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

int KcpInterface::init()
{
    QString systemInfo;
    if(!InfoDbus::SystemInfo::getSystemInfo(1,systemInfo) || !InfoDbus::SystemInfo::getSystemInfo(0,systemInfo))
    {
        KLOG_DEBUG() << "Connect dbus service failed! ";
        return -1;
    }
    //加载翻译文件
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-system",
                            ".",
                             TRANSLATION_DIR,
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel system load translation failed" ;
        m_translator->deleteLater();
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }
    return 0;
}

