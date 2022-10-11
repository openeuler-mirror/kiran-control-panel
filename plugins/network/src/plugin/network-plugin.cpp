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
#include "details-page.h"
#include "cpanel-network-widget.h"
//#include "network-config.h"
#include "config.h"
using namespace NetworkManager;

NetworkPlugin::NetworkPlugin()
{
}

NetworkPlugin::~NetworkPlugin()
{
}

int NetworkPlugin::init()
{
    loadTranslator();
    return 0;
}

void NetworkPlugin::uninit()
{
}

QWidget *NetworkPlugin::getSubItemWidget(QString subItemName)
{
    QWidget* widget = nullptr;
    if (subItemName == "Network")
    {
        widget = new CPanelNetworkWidget;
    }
    m_currentWidget = widget;
    return m_currentWidget;
}

bool NetworkPlugin::haveUnsavedOptions()
{
    return false;
}

QStringList NetworkPlugin::visibleSubItems()
{
    return QStringList() << "Network";;
}

void NetworkPlugin::loadTranslator()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator(qApp);
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-network",
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
}

