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
#include "timedate-plugin.h"
#include "kiran-timedate-global-data.h"
#include "timedate-subitem.h"

#include <kiran-log/qt5-log-i.h>
#include <QCoreApplication>
#include <QTranslator>

TimeDatePlugin::TimeDatePlugin(QObject* parent)
    : QObject(parent)
{
}

TimeDatePlugin ::~TimeDatePlugin()
{
}

int TimeDatePlugin::init(KiranControlPanel::PanelInterface* interface)
{
    m_panelInterface = interface;

    if (!KiranTimeDateGlobalData::instance()->init())
    {
        KLOG_ERROR() << "failed to load time zone information";
        return -1;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-timedate",
                            ".",
                            "/usr/share/kiran-cpanel-timedate/translations/",
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

    m_subitem.reset(new TimeDateSubItem());
    return 0;
}

void TimeDatePlugin::uninit()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        m_translator->deleteLater();
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> TimeDatePlugin::getSubItems()
{
    return {m_subitem};
}