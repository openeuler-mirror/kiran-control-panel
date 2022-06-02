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

CPanelPluginManager* CPanelPluginManager::_instance = nullptr;

CPanelPluginManager* CPanelPluginManager::getInstance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    if (Q_UNLIKELY(!_instance))
    {
        if (!_instance)
        {
            _instance = new CPanelPluginManager;
        }
    }

    return _instance;
}

void CPanelPluginManager::deleteInstance()
{
    if(!_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}

CPanelPluginManager::CPanelPluginManager()
{
}

CPanelPluginManager::~CPanelPluginManager()
{
    clear();
}

void CPanelPluginManager::loadAll()
{
    clear();

    CategoryInfoList tempCategoryList;

    ///读出所有的分类信息
    QDir categoryDesktopDir(CATEGORY_DESKTOP_DIR);
    QFileInfoList fileInfoList = categoryDesktopDir.entryInfoList({"*.desktop"}, QDir::Files);
    Q_FOREACH (auto categoryFileInfo, fileInfoList)
    {
        auto ret = CPanelCategoryInfo::loadCategory(categoryFileInfo.absoluteFilePath());
        if (ret == nullptr)
        {
            KLOG_ERROR() << "can't load category desktop <" << categoryFileInfo.absoluteFilePath() << ">!"
                           << ",ignore it!";
            continue;
        }
        KLOG_INFO() << "load category:" << ret->getCategoryDesktopInfo().categoryName;
        ///插入的时候就不进行排序了，后续加载完所有的分类进行排序，再形成category name和排序完的序列号的map，后续插件插入更快速
        tempCategoryList.append(ret);
    }

    ///根据权重对分类进行排序
    auto sortFunc = [](const QSharedPointer<CPanelCategoryInfo>& category_1,
                       const QSharedPointer<CPanelCategoryInfo>& category_2) -> bool {
        return category_1->getCategoryDesktopInfo().weight > category_2->getCategoryDesktopInfo().weight;
    };
    qSort(tempCategoryList.begin(), tempCategoryList.end(), sortFunc);

    ///记录下分类的ID和分类列表中序号对应关系
    QMap<QString, int> categoryIdxMap;
    for (int i = 0; i < tempCategoryList.count(); i++)
    {
        const auto& category = tempCategoryList.at(i);
        KLOG_INFO() << i << "->" << category->getCategoryDesktopInfo().categoryName;
        categoryIdxMap[category->getCategoryDesktopInfo().categoryName] = i;
    }

    ///读出所有的插件信息
    QDir pluginDesktopDir(PLUGIN_DESKTOP_DIR);
    QFileInfoList pluginFileInfoList = pluginDesktopDir.entryInfoList({"*.desktop"}, QDir::Files);
    Q_FOREACH (auto pluginFileInfo, pluginFileInfoList)
    {
        auto ret = CPanelPluginHelper::loadPlugin(pluginFileInfo.absoluteFilePath());
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
                           << "can't find category[" << pluginDesktopInfo.category;
            continue;
        }
        int categoryIdx = categoryIdxMapIter.value();
        tempCategoryList.at(categoryIdx)->insertPlugin(ret);
        KLOG_INFO() << "load plugin:" << pluginDesktopInfo.name;
    }

    m_categoryInfos.swap(tempCategoryList);
}

CPanelPluginManager::CategoryInfoList CPanelPluginManager::getCategorys()
{
    return m_categoryInfos;
}

void CPanelPluginManager::clear()
{
    m_categoryInfos.clear();
}