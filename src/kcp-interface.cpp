/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kcp-interface.h"

#include "config.h"
#include "kiran-display-configuration.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QDBusInterface>
#include <QPushButton>
#include <QTranslator>

#define KCP_SUBITEM_DISPLAY_ID "Display"

KcpInterface::KcpInterface()
{
}

KcpInterface::~KcpInterface()
{
}

int KcpInterface::init()
{
    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-display",
                            ".",
                            TRANSLATION_DIR_PATH,
                            ".qm"))
    {
        m_translator->deleteLater();
        m_translator = nullptr;
        KLOG_ERROR_S() << "load translator failed!";
        return -1;
    }
    qApp->installTranslator(m_translator);
    return 0;
}

void KcpInterface::uninit()
{
    if (m_translator)
    {
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QWidget* KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;

    if (id == KCP_SUBITEM_DISPLAY_ID)
    {
        widget = new KiranDisplayConfiguration();
    }

    return widget;
}

bool KcpInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpInterface::visibleSubItems()
{
    return QStringList() << KCP_SUBITEM_DISPLAY_ID;
}