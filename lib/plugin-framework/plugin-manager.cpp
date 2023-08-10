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
#include "plugin-manager.h"
#include "plugin-loader.h"

#include <QMutex>
#include <QVector>

PluginManager* PluginManager::_instance = nullptr;

PluginManager::~PluginManager()
{
    qDeleteAll(m_plugins);
}

PluginManager* PluginManager::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    if (Q_UNLIKELY(!_instance))
    {
        if (!_instance)
        {
            _instance = new PluginManager();
        }
    }

    return _instance;
}

bool PluginManager::init()
{
    if (m_isInited)
    {
        return true;
    }

    m_plugins = PluginLoader::loadAllPlugins();
    m_isInited = true;
    return true;
}

void PluginManager::dump()
{
    for (auto plugin : m_plugins)
    {
        fprintf(stdout, "plugin -- id: %-20s    name: %-20s\n",
                plugin->getID().toStdString().c_str(),
                plugin->getName().toStdString().c_str());
        auto subitemList = plugin->getSubItems();
        for (auto subitem : subitemList)
        {
            fprintf(stdout, "\tsubitem id: %-15s    name: %-15s\n",
                    subitem->getID().toStdString().c_str(),
                    subitem->getName().toStdString().c_str());
        }
    }
}

QList<Plugin*> PluginManager::getPlugins()
{
    return m_plugins;
}
