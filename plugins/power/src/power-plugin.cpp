/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "power-plugin.h"
#include "battery-subitem.h"
#include "config.h"
#include "general-settings-subitem.h"
#include "power-subitem.h"
#include "upower-interface.h"
#include "src/dbus/power.h"

#include <kiran-log/qt5-log-i.h>
#include <QCoreApplication>
#include <QTranslator>

PowerPlugin::PowerPlugin(QObject* parent)
    : QObject(parent)
{
}

PowerPlugin::~PowerPlugin()
{
}

int PowerPlugin::init(KiranControlPanel::PanelInterface* interface)
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
                            TRANSLATE_PREFIX,
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
    
    PowerInterface::globalInit();

    m_subitems.append(KiranControlPanel::SubItemPtr(new GeneralSettingsSubItem()));
    m_subitems.append(KiranControlPanel::SubItemPtr(new PowerSubItem()));
    // if (UPowerInterface::haveBattery())
    {
        m_subitems.append(KiranControlPanel::SubItemPtr(new BatterySubItem()));
    }

    return 0;
}

void PowerPlugin::uninit()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    PowerInterface::globalDeinit();
}

QVector<KiranControlPanel::SubItemPtr> PowerPlugin::getSubItems()
{
    return m_subitems;
}
