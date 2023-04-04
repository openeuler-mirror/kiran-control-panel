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
#include <QEvent>
#include <QMap>

GroupSubItem::GroupSubItem(QObject* parent)
    : QObject(parent)
{
}

GroupSubItem::~GroupSubItem()
{
}

bool GroupSubItem::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_kiranGroupManager && event->type() == QEvent::Destroy)
    {
        m_kiranGroupManager = nullptr;
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
    m_kiranGroupManager = new KiranGroupManager();
    m_kiranGroupManager->installEventFilter(this);
    return m_kiranGroupManager;
}

QVector<QPair<QString, QString>> GroupSubItem::getSearchKeys()
{
     return {{tr("Creat group"), "CreatGroup"},
            {tr("Change group name"), "ChangeGroupName"},
            {tr("Add group member"), "AddGroupMember"}};
}

bool GroupSubItem::jumpToSearchEntry(const QString& key)
{ 
        if (!m_kiranGroupManager)
            return false;

        return true;
}
