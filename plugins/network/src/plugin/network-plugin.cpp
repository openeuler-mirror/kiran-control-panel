/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#include "network-plugin.h"
#include <qt5-log-i.h>
#include <QCoreApplication>
#include "config.h"
#include "cpanel-network-widget.h"
#include "network-subitem.h"

NetworkPlugin::NetworkPlugin(QObject* parent) : QObject(parent)
{
}

NetworkPlugin::~NetworkPlugin()
{
}

int NetworkPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator(qApp);
    if (!m_translator->load(QLocale(),
                            "kiran-control-panel",
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

    auto networkSubItem = new NetworkSubItem(interface,this);
    m_subitem.reset(networkSubItem);

    return 0;
}

void NetworkPlugin::uninit()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> NetworkPlugin::getSubItems()
{
    return {m_subitem};
}
