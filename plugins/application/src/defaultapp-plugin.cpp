/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#include "defaultapp-plugin.h"
#include "config.h"
#include "defaultapp-subitem.h"
#include "defaultapp.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>
#include <QCoreApplication>
#include <tuple>

DefaultappPlugin::DefaultappPlugin(QObject* parent) : QObject(parent)
{
}

DefaultappPlugin::~DefaultappPlugin()
{
}

int DefaultappPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    initTranslator();
    initSubItem();
    return 0;
}

void DefaultappPlugin::uninit()
{
    if (m_translator)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> DefaultappPlugin::getSubItems()
{
    return m_subitems;
}

void DefaultappPlugin::initSubItem()
{
    auto defaultAppSubItemCreater = []() -> QWidget*
    {
        return new DefaultApp();
    };
    struct SubItemStruct
    {
        QString id;
        QString name;
        QString category;
        QString desc;
        QString icon;
        int weight;
        CreateWidgetFunc func;
    };
    QList<SubItemStruct> subitemInfos = {
        {"DefaultApp",
         tr("DefaultApp"),
         "app-manager",
         "",
         ":/kcp-defaultapp-images/defaultapp.svg",
         99,
         defaultAppSubItemCreater},
        {"DefaultApp",
         tr("DefaultApp"),
         "app-manager",
         "",
         ":/kcp-defaultapp-images/defaultapp.svg",
         98,
         defaultAppSubItemCreater}};

    for (const SubItemStruct& subitemInfo : subitemInfos)
    {
        DefaultAppSubItem* subitem = new DefaultAppSubItem(subitemInfo.func);

        subitem->setID(subitemInfo.id);
        subitem->setName(subitemInfo.name);
        subitem->setCategory(subitemInfo.category);
        subitem->setDesc(subitemInfo.desc);
        subitem->setIcon(subitemInfo.icon);
        subitem->setWeight(subitemInfo.weight);
        m_subitems.append(KiranControlPanel::SubItemPtr(subitem));
    }
}

void DefaultappPlugin::initTranslator()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-defaultapp",
                            ".",
                            TRANSLATE_PREFIX,
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel defaultapp load translation failed";
        delete m_translator;
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }
}