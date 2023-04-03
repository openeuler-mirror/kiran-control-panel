/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-group is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
 * Mulan PSL v2 for more details.
 *
 * Author:     wangshichang <shichang@isrc.iscas.ac.cn>
 */

#include "group-plugin.h"
#include "accounts-global-info.h"
#include "config.h"
#include "group-subitem.h"
#include "groups-global-info.h"
#include "kiran-group-manager.h"
#include "plugin-group-config.h"

#include <qt5-log-i.h>
#include <QCoreApplication>
#include <QList>
#include <QLocale>
#include <QTranslator>

GroupPlugin::GroupPlugin(QObject *parent)
    : QObject(parent)
{
}

GroupPlugin::~GroupPlugin()
{
}

int GroupPlugin::init(KiranControlPanel::PanelInterface *interface)
{
    m_panelInterface = interface;
    if (!GroupsGlobalInfo::instance()->init())
    {
        KLOG_ERROR() << "load group info failed!";
        return -1;
    }
    if (!AccountsGlobalInfo::instance()->init())
    {
        KLOG_ERROR() << "load user info failed!";
        return -1;
    }
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(), "kiran-cpanel-group", ".",
                            PLUGIN_TRANSLATION_DIR, ".qm"))
    {
        m_translator->deleteLater();
        m_translator = nullptr;
        KLOG_ERROR() << "load translator failed!";
    }
    else
    {
        qApp->installTranslator(m_translator);
    }

    m_subitem.reset(new GroupSubItem());
    return 0;
}

void GroupPlugin::uninit()
{
    if (m_translator)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> GroupPlugin::getSubItems()
{
    return {m_subitem};
}
