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

#include "group-subitem.h"
#include "group-page.h"

#include <QEvent>

#define GROUP_SEARCH_COMMAND_CREATE_GROUP "@CreateGroup"
#define GROUP_SEARCH_COMMAND_JUMP_TO_GROUP_INFO "@GroupInfo"

GroupSubItem::GroupSubItem(QObject* parent)
    : QObject(parent)
{
}

GroupSubItem::~GroupSubItem()
{
}

bool GroupSubItem::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_groupPage && event->type() == QEvent::Destroy)
    {
        m_groupPage = nullptr;
    }

    return QObject::eventFilter(watched, event);
}

QString GroupSubItem::getID()
{
    return "Group";
}

QString GroupSubItem::getName()
{
    return tr("Group");
}

QString GroupSubItem::getCategory()
{
    return "group";
}

QString GroupSubItem::getDesc()
{
    return "";
}

QString GroupSubItem::getIcon()
{
    return "group";
}

int GroupSubItem::getWeight()
{
    return 4;
}

QWidget* GroupSubItem::createWidget()
{
    m_groupPage = new GroupPage();
    m_groupPage->installEventFilter(this);
    return m_groupPage;
}

QVector<QPair<QString, QString>> GroupSubItem::getSearchKeys()
{
    return {{tr("Create group"), GROUP_SEARCH_COMMAND_CREATE_GROUP},
            {tr("Group information"), GROUP_SEARCH_COMMAND_JUMP_TO_GROUP_INFO}};
}

bool GroupSubItem::jumpToSearchEntry(const QString& key)
{
    if (!m_groupPage)
        return false;

    if (key == GROUP_SEARCH_COMMAND_CREATE_GROUP)
    {
        m_groupPage->jumpToPage(GroupPage::PAGE_CREATE_GROUP);
    }
    else if (key == GROUP_SEARCH_COMMAND_JUMP_TO_GROUP_INFO)
    {
        m_groupPage->jumpToPage(GroupPage::PAGE_GROUP_INFO);
    }
    else
    {
        return false;
    }

    return true;
}
