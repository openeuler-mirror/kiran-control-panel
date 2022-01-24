/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-cpanel-power is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "kcp-power-interface.h"
#include "battery-settings-page.h"
#include "config.h"
#include "general-settings-page.h"
#include "power-settings-page.h"
#include "upower-interface.h"

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
    QStringList subItem({{"GeneralSettings"},{"PowerSettings"}});

    if( UPowerInterface::haveBattery() )
    {
        subItem << "BatterySettings";
    }

    return subItem;
}
