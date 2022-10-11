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
#include "list-widget-subitem.h"

#define ROLE_SORT_WEIGHT Qt::UserRole + 669

ListWidgetSubItem::ListWidgetSubItem(QListWidget *view, int type)
    : KiranSidebarItem(view, type)
{
}

ListWidgetSubItem::ListWidgetSubItem(const QString &text, QListWidget *view, int type)
    : KiranSidebarItem(text, view, type)
{
}

ListWidgetSubItem::ListWidgetSubItem(const QIcon &icon, const QString &text, QListWidget *view, int type)
    : KiranSidebarItem(icon, text, view, type)
{
}

ListWidgetSubItem::ListWidgetSubItem(const QListWidgetItem &other)
    : KiranSidebarItem(other)
{
}

ListWidgetSubItem::~ListWidgetSubItem()
{
}

bool ListWidgetSubItem::operator<(const QListWidgetItem &other) const
{
    bool toInt = false;

    int weight = data(ROLE_SORT_WEIGHT).toInt(&toInt);
    if (!toInt)
        weight = 0;

    int otherWeight = other.data(ROLE_SORT_WEIGHT).toInt(&toInt);
    if (!toInt)
        otherWeight = 0;

    return weight < otherWeight;
}

int ListWidgetSubItem::getWeight()
{
    QVariant var = data(ROLE_SORT_WEIGHT);
    bool toInt = false;
    int weight = var.toInt(&toInt);
    return toInt ? weight : 0;
}

void ListWidgetSubItem::setWeight(int weight)
{
    setData(ROLE_SORT_WEIGHT, QVariant(weight));
}
