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

#include "plugin-loader.h"
#include "config.h"
#include "plugin-v1.h"
#include "plugin-v2.h"

#include <qt5-log-i.h>
#include <QDir>
#include <QScopedPointer>

QList<Plugin*> PluginLoader::loadAllPlugins()
{
    QStringList loadedPluginLibrarys;
    QList<Plugin*> plugins;

    // 老版本接口,通过插件desktop文件加载插件信息以及插件的共享库
    QDir desktopDir(PLUGIN_DESKTOP_DIR);
    QFileInfoList desktopFileInfoList = desktopDir.entryInfoList({"*.desktop"}, QDir::Files);
    for (auto desktopFileInfo : desktopFileInfoList)
    {
        QString pluginPath = desktopFileInfo.absoluteFilePath();

        QScopedPointer<PluginV1> pPlugin(new PluginV1());
        if (!pPlugin->load(pluginPath))
        {
            KLOG_WARNING() << "PluginLoader: can't load" << pluginPath;
            continue;
        }

        QString libraryPath = pPlugin->getLibraryPath();
        loadedPluginLibrarys << libraryPath;
        plugins << pPlugin.take();
    }

    // 新版本接口，直接加载插件共享库
    QDir libraryDir(PLUGIN_LIBRARY_DIR);
    QFileInfoList libraryFileInfoList = libraryDir.entryInfoList({"*.so"}, QDir::Files);
    for (auto libraryFileInfo : libraryFileInfoList)
    {
        QString libraryPath = libraryFileInfo.absoluteFilePath();

        if (loadedPluginLibrarys.contains(libraryPath))
            continue;

        QScopedPointer<PluginV2> pPlugin(new PluginV2());
        if (!pPlugin->load(libraryPath))
        {
            KLOG_WARNING() << "PluginLoader: can't load" << libraryPath;
            continue;
        }
        plugins << pPlugin.take();
    }

    return plugins;
}

Plugin* PluginLoader::loadSinglePlugin(const QString& path)
{
    if (path.endsWith(".desktop"))
    {
        QScopedPointer<PluginV1> pPlugin(new PluginV1);
        if (pPlugin->load(path))
        {
            return pPlugin.take();
        }
    }
    else if (path.endsWith(".so"))
    {
        QScopedPointer<PluginV2> pPlugin(new PluginV2);
        if (pPlugin->load(path))
        {
            return pPlugin.take();
        }
    }

    return nullptr;
}