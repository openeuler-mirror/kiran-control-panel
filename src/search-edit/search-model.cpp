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

#include "search-model.h"
#include "category-manager.h"
#include "category.h"

#include <qt5-log-i.h>

const int SearchModel::roleCategoryID = Qt::UserRole + 1;
const int SearchModel::roleSubItemID = Qt::UserRole + 2;
const int SearchModel::roleSearchKey = Qt::UserRole + 3;

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
}

void SearchModel::loadSearchModel()
{
    clear();

    static bool needConnect = true;
    auto categorys = CategoryManager::instance()->getCategorys();
    for (auto category : categorys)
    {
        QString categoryName = category->getName();
        QString categoryID = category->getID();

        auto subitems = category->getSubItems();
        if( subitems.size() == 0 )
        {
            continue;
        }

        // 添加分类搜索项
        appendItem(categoryName, categoryID, subitems.at(0)->getID());

        /**
         * NOTE: 分类下单个子功能项应不添加搜索项
         * 避免 显示设置分类下显示设置子功能项，构成"显示设置->显示设置"这种搜索项
        */
        bool addSubItemNamePrefix = true;
        if (subitems.size() == 1)
        {
            addSubItemNamePrefix = false;
        }

        for (auto subitem : subitems)
        {
            QString subitemName = subitem->getName();
            QString subitemID = subitem->getID();

            QString subItemPrefix;
            if (addSubItemNamePrefix)
            {
                QString subItemSearchKey = QString("%1 -> %2").arg(categoryName).arg(subitemName);
                appendItem(subItemSearchKey, categoryID, subitemID);

                subItemPrefix = subItemSearchKey;
            }
            else
            {
                subItemPrefix = categoryName;
            }

            auto searchItems = subitem->getSearchKeys();
            for (auto searchItem : searchItems)
            {
                QString searchName = searchItem.first;
                QString searchKey = searchItem.second;

                QString searchText = QString("%1 -> %2").arg(subItemPrefix).arg(searchName);
                appendItem(searchText, categoryID, subitemID, searchKey);
            }
        }

        // 第一次加载搜索模型,连接到每一个分类下
        if (needConnect)
        {
            connect(category, &Category::subItemAdded, this, &SearchModel::handleSubItemAdded);
            connect(category, &Category::subItemDeleted, this, &SearchModel::handleSubItemDeleted);
            connect(category, &Category::subItemInfoChanged, this, &SearchModel::handleSubItemInfoChanged);
        }
    }

    needConnect = false;
}

void SearchModel::appendItem(const QString &text, const QString &category, const QString &subItem, const QString &searchKey)
{
    auto item = new QStandardItem(text);

    item->setData(category, roleCategoryID);
    item->setData(subItem, roleSubItemID);
    item->setData(searchKey, roleSearchKey);

    appendRow(item);
}

void SearchModel::handleSubItemsChanged()
{
    loadSearchModel();
}

void SearchModel::handleSubItemAdded(const QString &id)
{
    Category *category = qobject_cast<Category *>(sender());
    KiranControlPanel::SubItemPtr subitem = category->getSubItem(id);

    qDebug() << "search model subitem added:" << category->getName() << subitem->getName();
    loadSearchModel();
    qDebug() << "search model subitem added:" << category->getName() << subitem->getName() << "finished!";
}

void SearchModel::handleSubItemDeleted(const QString &id)
{
    Category *category = qobject_cast<Category *>(sender());
    KiranControlPanel::SubItemPtr subitem = category->getSubItem(id);

    qDebug() << "search model subitem deleted:" << category->getName() << subitem->getName();
    loadSearchModel();
    qDebug() << "search model subitem deleted:" << category->getName() << subitem->getName() << "finished!";
}

void SearchModel::handleSubItemInfoChanged(const QString &subitemID)
{
    loadSearchModel();
}