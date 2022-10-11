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

#include "category.h"
#include "qt5-log-i.h"

Category::Category(QObject* parent)
{
}

Category::~Category()
{
}

void Category::setName(const QString& name)
{
    m_name = name;
}

QString Category::getName() const
{
    return m_name;
}

void Category::setID(const QString& id)
{
    m_id = id;
}

QString Category::getID() const
{
    return m_id;
}

void Category::setIcon(const QString& icon)
{
    m_icon = icon;
}

QString Category::getIcon() const
{
    return m_icon;
}

void Category::setWeight(int weight)
{
    m_weight = weight;
}

int Category::getWeight() const
{
    return m_weight;
}

QStringList Category::getSubItemIDs()
{
    QReadLocker locker(&m_rwLock);

    QStringList subitemIds;
    for (auto subitem : m_subitems)
    {
        subitemIds << subitem->getID();
    }

    return subitemIds;
}

QVector<KcpPluginSubItemPtr> Category::getSubItems()
{
    return m_subitems;
}

KcpPluginSubItemPtr Category::getSubItem(const QString& subitemID)
{
    auto iter = m_subitemIDMap.find(subitemID);
    if (iter == m_subitemIDMap.end())
    {
        KLOG_WARNING() << "Category:" << getName()
                       << "can't find subitem:" << subitemID;
        return KcpPluginSubItemPtr();
    }
    else
    {
        return *iter;
    }
}

void Category::appendSubItem(KcpPluginSubItemPtr subitem)
{
    int weight = subitem->getWeight();

    QWriteLocker lcoker(&m_rwLock);

    auto iter = m_subitems.rbegin();
    for (; iter != m_subitems.rend(); iter++)
    {
        if (weight >= (*iter)->getWeight())
        {
            break;
        }
    }

    m_subitems.insert(iter.base(), subitem);
    m_subitemIDMap.insert(subitem->getID(), subitem);

    qDebug() << "Category:" << getName() << getID()
             << "appended subitem:" << subitem->getName() << subitem->getID();

    emit subItemAdded(subitem->getID());
}

void Category::appendSubItems(QVector<KcpPluginSubItemPtr> pluginSubItem)
{
    for (auto subitem : pluginSubItem)
    {
        appendSubItem(subitem);
    }
}

void Category::removeSubItem(KcpPluginSubItemPtr pluginSubItem)
{
    int idx = m_subitems.indexOf(pluginSubItem);
    m_subitems.removeAt(idx);
    m_subitemIDMap.remove(pluginSubItem->getID());

    qDebug() << "Category:" << getName() << getID()
             << "removed subitem:" << pluginSubItem->getName() << pluginSubItem->getID();

    emit subItemDeleted(pluginSubItem->getID());
}

void Category::removeSubItem(const QString& subitem)
{
    for (auto iter = m_subitems.begin(); iter != m_subitems.end(); iter++)
    {
        if ((*iter)->getID() == subitem)
        {
            removeSubItem((*iter));
            break;
        }
    }
}