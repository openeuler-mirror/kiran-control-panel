#include "interface.h"
#include "pages/hardware-information/hardware-information-widget.h"
#include "pages/system-information/system-information-widget.h"
#include "system-info-dbus.h"
#include "config/config.h"
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QLocale>
#include <QTranslator>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
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
        std::cout << "Kiran cpanel system load translation failed" ;
        delete m_translator;
        m_translator = nullptr;
        return -1;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }
    return 0;
}

