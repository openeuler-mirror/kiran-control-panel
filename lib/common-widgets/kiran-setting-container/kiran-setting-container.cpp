/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "kiran-setting-container.h"
#include <QBoxLayout>
#include <QScrollArea>

void SettingsContainer::delAllItemOfLayout(QLayout* layout)
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
            delAllItemOfLayout(child->layout());
        }

        delete child;
    }
}

SettingsContainer::SettingsContainer(QWidget* parent)
    : QWidget(parent)
{
    init();
    adjustSizeToItmes();
}

SettingsContainer::~SettingsContainer()
{
}

void SettingsContainer::addKiranSettingItem(QWidget* widget)
{
    m_containerLayout->addWidget(widget, 0);
    adjustSizeToItmes();
}

void SettingsContainer::clear()
{
    delAllItemOfLayout(m_containerLayout);
    adjustSizeToItmes();
}

void SettingsContainer::init()
{
    m_mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    auto scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    scrollarea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    scrollarea->setFrameStyle(QFrame::NoFrame);
    scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_mainLayout->addWidget(scrollarea);

    auto contentWidget = new QWidget(this);
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_containerLayout = new QBoxLayout(QBoxLayout::TopToBottom, contentWidget);
    m_containerLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_containerLayout->setSpacing(10);
    m_containerLayout->setContentsMargins(0, 0, 0, 0);
    scrollarea->setWidget(contentWidget);
}

void SettingsContainer::adjustSizeToItmes()
{
    auto mainMargin = m_mainLayout->contentsMargins();
    auto contentMargin = m_containerLayout->contentsMargins();
    auto contentSpacing = m_containerLayout->spacing();

    int height = mainMargin.top() + mainMargin.bottom() + contentMargin.top() + contentMargin.bottom();
    height += contentSpacing * (m_containerLayout->count() - 1);
    height += 36 * m_containerLayout->count();
    setMaximumHeight(height > 0 ? height : 0);
}
