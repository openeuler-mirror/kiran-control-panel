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
#include "systeminfo-plugin.h"

#include "hardware-subitem.h"
#include "system-subitem.h"

#include <kiran-log/qt5-log-i.h>
#include <QCoreApplication>
#include <QTranslator>

#define TRANSLATION_DIR "/usr/share/kiran-control-panel/translations"

SystemInfoPlugin::SystemInfoPlugin(QObject* parent)
    : QObject(parent)
{
}

SystemInfoPlugin::~SystemInfoPlugin()
{
}

int SystemInfoPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    m_subitems.append(KiranControlPanel::SubItemPtr(new HardwareSubItem));
    m_subitems.append(KiranControlPanel::SubItemPtr(new SystemInfoSubItem));
    return 0;
}

void SystemInfoPlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> SystemInfoPlugin::getSubItems()
{
    return m_subitems;
}