//
// Created by liuxinhao on 2022/6/16.
//

#include "search-model.h"
#include "plugin-manager.h"

#include <qt5-log-i.h>

const int SearchModel::roleCategoryID = Qt::UserRole+1;
const int SearchModel::roleSubItemID = Qt::UserRole+2;

SearchModel::SearchModel(QObject *parent)
{
    init();
}

SearchModel::~SearchModel()
{

}

void SearchModel::init()
{
    //从插件信息之中构建搜索模型
    loadSearchModel();

    //连接插件
    auto categories = PluginManager::getInstance()->getCategorys();
    for( auto &&category:categories )
    {
        auto&& plugins = category->getPlugins();
        for( auto &&plugin:plugins )
        {
            connect(plugin.data(),&PluginHelper::visibleSubItemsChanged,this,&SearchModel::handlePluginVisibleSubItemChanged);
        }
    }
}

void SearchModel::loadSearchModel()
{
    clear();

    auto categories = PluginManager::getInstance()->getCategorys();
    for( auto &&category:categories)
    {
        auto categoryInfo = category->getCategoryDesktopInfo();
        auto && plugins = category->getPlugins();
        for(auto &&plugin:plugins)
        {
            auto pluginInfo = plugin->getPluginDesktopInfo();
            auto pluginVisibleSubItems = plugin->visibleSubItems();
            auto pluginSubItems = pluginInfo.subItems;
            for( auto subItem:pluginSubItems )
            {
                if( !pluginVisibleSubItems.contains(subItem.id) )
                    continue ;
                appendItem(QString("%1 -> %2").arg(categoryInfo.name).arg(subItem.name),categoryInfo.categoryName,subItem.id);
            }
        }
    }
}

void SearchModel::appendItem(const QString &text, const QString &category, const QString &subItem)
{
    auto item = new QStandardItem(text);
    item->setData(category,roleCategoryID);
    item->setData(subItem,roleSubItemID);
    appendRow(item);
}

void SearchModel::handlePluginVisibleSubItemChanged()
{
    loadSearchModel();
}