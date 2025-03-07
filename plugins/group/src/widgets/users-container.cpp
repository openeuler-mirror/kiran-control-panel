/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "users-container.h"
#include <qt5-log-i.h>
#include <QBoxLayout>
#include <QResizeEvent>
#include <QScrollArea>
#include "user-list-item.h"
void deleteAllItemOfLayout(QLayout* layout)
{
    QLayoutItem* child = nullptr;
    while ((child = layout->takeAt(0)) != 0)
    {
        if (child->widget())
        {
            child->widget()->setParent(NULL);
            delete child->widget();
        }
        else if (child->layout())
        {
            deleteAllItemOfLayout(child->layout());
        }

        delete child;
        child = nullptr;
    }
}

UsersContainer::UsersContainer(QWidget* parent)
    : QWidget(parent)
{
    init();
    adjustSizeToItmes();
}

UsersContainer::~UsersContainer()
{
}

void UsersContainer::addItem(UserListItem* widget)
{
    m_containerLayout->addWidget(widget, 0);
    adjustSizeToItmes();
}

void UsersContainer::setItemVisible(const QString& name, bool visible)
{
    for (int i = 0; i < m_containerLayout->count(); ++i)
    {
        auto layoutItem = m_containerLayout->itemAt(i);
        if (layoutItem && layoutItem->widget())
        {
            auto item = qobject_cast<UserListItem*>(layoutItem->widget());
            if (item->name() == name)
            {
                item->setVisible(visible);
                break;
            }
        }
    }
}

QList<UserListItem*> UsersContainer::getSelectedItems()
{
    QList<UserListItem*> result;
    for (int i = 0; i < m_containerLayout->count(); ++i)
    {
        auto layoutItem = m_containerLayout->itemAt(i);
        if (layoutItem && layoutItem->widget())
        {
            auto item = qobject_cast<UserListItem*>(layoutItem->widget());
            if (item && item->isSelected())
            {
                result.append(item);
            }
        }
    }
    return result;
}

void UsersContainer::clear()
{
    deleteAllItemOfLayout(m_containerLayout);
    adjustSizeToItmes();
}

void UsersContainer::init()
{
    m_mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_mainLayout->addWidget(m_scrollArea);

    m_contentWidget = new QWidget(this);
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_containerLayout = new QBoxLayout(QBoxLayout::TopToBottom, m_contentWidget);
    m_containerLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_containerLayout->setSpacing(10);
    m_containerLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(m_contentWidget);
}

void UsersContainer::adjustSizeToItmes()
{
    auto mainMargin = m_mainLayout->contentsMargins();
    auto contentMargin = m_containerLayout->contentsMargins();
    auto contentSpacing = m_containerLayout->spacing();

    int height = mainMargin.top() + mainMargin.bottom() + contentMargin.top() + contentMargin.bottom();
    height += contentSpacing * (m_containerLayout->count() - 1);
    height += 36 * m_containerLayout->count();
    m_contentWidget->setMaximumHeight(height > 0 ? height : 0);
}
