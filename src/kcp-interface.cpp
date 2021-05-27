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
        KLOG_ERROR_S() << "failed to load time zone information";
        return -1;
    }

    return 0;
}

void KcpInterface::uninit()
{
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
            KLOG_WARNING_S() << "can't load stylesheet from file" << DEFAULT_STYLE_SHEET_FILE;
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
