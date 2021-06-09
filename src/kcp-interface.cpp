//
// Created by lxh on 2021/5/26.
//

#include "kcp-interface.h"
#include "kiran-timedate-widget.h"
#include "kiran-timedate-global-data.h"

#include <QFile>
#include <qt5-log-i.h>

//日期时间功能项ID
#define KCP_SUBITEM_DATETIME_ID "Datetime"

//默认样式表文件
#define DEFAULT_STYLE_SHEET_FILE ":/kcp-datetime-themes/black-theme.qss"

KcpInterface::~KcpInterface()
{
}

int KcpInterface::init()
{
    //TODO:后续考虑全局数据的创建和释放与控件的生存周期绑定
    ///加载全局数据
    if( !KiranTimeDateGlobalData::instance()->init() ){
        KLOG_ERROR() << "failed to load time zone information";
        return -1;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-datetime",
                            ".",
                            "/usr/share/kiran-cpanel-datetime/translations/",
                            ".qm"))
    {
        m_translator->deleteLater();
        m_translator = nullptr;
        KLOG_ERROR() << "load translator failed!";
    }
    else
    {
        qApp->installTranslator(m_translator);
    }

    return 0;
}

void KcpInterface::uninit()
{
    if(m_translator)
    {
        QCoreApplication::removeTranslator(m_translator);
        m_translator->deleteLater();
        m_translator = nullptr;
    }
}

QWidget* KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if (id == KCP_SUBITEM_DATETIME_ID)
    {
        widget = new KiranTimeDateWidget;

        QFile file(DEFAULT_STYLE_SHEET_FILE);
        if (file.open(QIODevice::ReadOnly))
        {
            QString stylesheet = file.readAll();
            widget->setStyleSheet(widget->styleSheet()+stylesheet);
            file.close();
        }
        else
        {
            KLOG_WARNING() << "can't load stylesheet from file" << DEFAULT_STYLE_SHEET_FILE;
        }
    }
    m_currentWidget = widget;
    return m_currentWidget;
}

bool KcpInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpInterface::visibleSubItems()
{
    return QStringList() << KCP_SUBITEM_DATETIME_ID;
}
