/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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

#include "plugin-manager.h"
#include "category-info.h"
#include "config.h"

#include <qt5-log-i.h>
#include <QDirIterator>
#include <QMutex>
#include <QScopedPointer>

PluginManager* PluginManager::_instance = nullptr;

PluginManager* PluginManager::getInstance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    if (Q_UNLIKELY(!_instance))
    {
        if (!_instance)
        {
            _instance = new PluginManager;
        }
    }

    return _instance;
}

void PluginManager::deleteInstance()
{
    if (!_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}

PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
    clear();
}

void PluginManager::loadAll()
{
    clear();
    CategoryInfoPtrList tempCategoryList;

    /// 读出所有的分类信息
    QDir categoryDesktopDir(CATEGORY_DESKTOP_DIR);
    QFileInfoList fileInfoList = categoryDesktopDir.entryInfoList({"*.desktop"}, QDir::Files);
    Q_FOREACH (auto categoryFileInfo, fileInfoList)
    {
        auto ret = CategoryInfo::loadCategory(categoryFileInfo.absoluteFilePath());
        if (ret == nullptr)
        {
            KLOG_ERROR() << "can't load category desktop <" << categoryFileInfo.absoluteFilePath() << ">!"
                         << ",ignore it!";
            continue;
        }
        /// 插入的时候就不进行排序了，后续加载完所有的分类进行排序，再形成category name和排序完的序列号的map，后续插件插入更快速
        tempCategoryList.append(ret);
    }

    /// 根据权重对分类进行排序
    auto sortFunc = [](const QSharedPointer<CategoryInfo>& category_1,
                       const QSharedPointer<CategoryInfo>& category_2) -> bool
    {
        return category_1->getCategoryDesktopInfo().weight > category_2->getCategoryDesktopInfo().weight;
    };
    std::sort(tempCategoryList.begin(),tempCategoryList.end(),sortFunc);

    /// 记录下分类的ID和分类列表中序号对应关系
    QMap<QString, int> categoryIdxMap;
    for (int i = 0; i < tempCategoryList.count(); i++)
    {
        const auto& category = tempCategoryList.at(i);
        categoryIdxMap[category->getCategoryDesktopInfo().categoryName] = i;
    }

    /// 读出所有的插件信息
    QDir pluginDesktopDir(PLUGIN_DESKTOP_DIR);
    QFileInfoList pluginFileInfoList = pluginDesktopDir.entryInfoList({"*.desktop"}, QDir::Files);
    Q_FOREACH (auto pluginFileInfo, pluginFileInfoList)
    {
        auto ret = PluginHelper::loadPlugin(pluginFileInfo.absoluteFilePath());
        if (ret == nullptr)
        {
            KLOG_ERROR() << "can't load plugin <" << pluginFileInfo.absoluteFilePath() << ">!"
                         << ",ignore it!";
            continue;
        }

        auto pluginDesktopInfo = ret->getPluginDesktopInfo();
        auto categoryIdxMapIter = categoryIdxMap.find(pluginDesktopInfo.category);
        if (categoryIdxMapIter == categoryIdxMap.end())
        {
            KLOG_ERROR() << "plugin desktop:" << pluginFileInfo.absoluteFilePath()
                         << "plugin name:" << pluginDesktopInfo.name
                         << "can't find category <" << pluginDesktopInfo.category << ">";
            continue;
        }
        int categoryIdx = categoryIdxMapIter.value();
        tempCategoryList.at(categoryIdx)->insertPlugin(ret);
    }
#if 1
    for(auto categoryInfo:tempCategoryList)
    {
        KLOG_DEBUG() << QString("category name: <%1> name: <%2> keywords: <%3> weight: <%4>")
                            .arg(categoryInfo->getCategoryDesktopInfo().categoryName)
                            .arg(categoryInfo->getCategoryDesktopInfo().name)
                            .arg(categoryInfo->getCategoryDesktopInfo().keywords.join(";"))
                            .arg(QString::number(categoryInfo->getCategoryDesktopInfo().weight));
        for(int i=0;i<categoryInfo->pluginsCount();i++)
        {
            auto plugin = categoryInfo->getPlugin(i);
            auto pluginInfo = plugin->getPluginDesktopInfo();
            KLOG_DEBUG() << "\t" << pluginInfo.name << pluginInfo.library;
        }
    }
#endif

    m_categoryInfos.swap(tempCategoryList);
}

CategoryInfoPtrList PluginManager::getCategorys()
{
    return m_categoryInfos;
}

void PluginManager::clear()
{
    m_categoryInfos.clear();
}