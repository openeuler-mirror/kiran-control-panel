//
// Created by liuxinhao on 2022/6/16.
//

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
    auto categorys = CategoryManager::instance()->getCategorys();
    for (auto category : categorys)
    {
        QString categoryName = category->getName();
        QString categoryID = category->getID();

        auto subitems = category->getSubItems();

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
                QString searchText = QString("%1 -> %2").arg(categoryName).arg(subitemName);
                appendItem(searchText, categoryID, subitemID);
                subItemPrefix = searchText;
            }
            else
            {
                QString searchText = categoryName;
                appendItem(searchText, categoryID, subitemID);
                subItemPrefix = searchText;
            }

            auto searchItems = subitem->getSearchKeys();
            for (auto searchItem : searchItems)
            {
                QString searchName = searchItem.first;
                QString searchKey = searchItem.second;
                QString searchText = QString("%1 : %2").arg(subItemPrefix).arg(searchName);
                appendItem(searchText, categoryID, subitemID, searchKey);
            }
        }

        static bool needConnect = true;
        if (needConnect)
        {
            connect(category, &Category::subItemAdded, this, &SearchModel::handleSubItemAdded);
            connect(category, &Category::subItemDeleted, this, &SearchModel::handleSubItemDeleted);
            connect(category, &Category::subItemInfoChanged, this, &SearchModel::handleSubItemInfoChanged);
            needConnect = false;
        }
    }
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
    KcpPluginSubItemPtr subitem = category->getSubItem(id);

    qDebug() << "search model subitem added:" << category->getName() << subitem->getName();
    loadSearchModel();
    qDebug() << "search model subitem added:" << category->getName() << subitem->getName() << "finished!";
}

void SearchModel::handleSubItemDeleted(const QString &id)
{
    Category *category = qobject_cast<Category *>(sender());
    KcpPluginSubItemPtr subitem = category->getSubItem(id);

    qDebug() << "search model subitem deleted:" << category->getName() << subitem->getName();
    loadSearchModel();
    qDebug() << "search model subitem deleted:" << category->getName() << subitem->getName() << "finished!";
}

void SearchModel::handleSubItemInfoChanged(const QString &subitemID)
{
    loadSearchModel();
}