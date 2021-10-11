//
// Created by lxh on 2021/5/21.
//

#include "kcp-power-interface.h"
#include "battery-settings-page.h"
#include "config.h"
#include "general-settings-page.h"
#include "power-settings-page.h"

#include <qt5-log-i.h>

KcpPowerInterface::~KcpPowerInterface()
{
}

int KcpPowerInterface::init()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator(qApp);
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-power",
                            ".",
                            KIRAN_POWER_MANAGER_TRANSLATIONS_DIR,
                            ".qm"))
    {
        KLOG_ERROR() << "can't load translator";
        delete m_translator;
        m_translator = nullptr;
    }
    else
    {
        qApp->installTranslator(m_translator);
    }

    return 0;
}

void KcpPowerInterface::uninit()
{
}

QWidget* KcpPowerInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if(id == "GeneralSettings")
    {
        widget = new GeneralSettingsPage;
    }
    else if (id == "PowerSettings")
    {
        widget = new PowerSettingsPage;
    }
    else if( id == "BatterySettings")
    {
        widget = new BatterySettingsPage;
    }
    return widget;
}

bool KcpPowerInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpPowerInterface::visibleSubItems()
{
    return QStringList() << "GeneralSettings" << "PowerSettings" << "BatterySettings";
}
