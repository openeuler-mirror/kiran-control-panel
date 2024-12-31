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
#include "category-side-bar.h"
#include "category-item.h"
#include "category-manager.h"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>

CategorySideBar::CategorySideBar(QWidget *parent)
    : KiranColorBlock(parent)
{
    init();
}

CategorySideBar::~CategorySideBar()
{
}

void CategorySideBar::init()
{
    setAccessibleName("ControlPanelCategory");

    // 布局
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    // 滚动区域
    auto pScrollArea = new QScrollArea(this);
    pScrollArea->setAutoFillBackground(false);
    pScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pScrollArea->setFrameStyle(QFrame::NoFrame);

    pScrollArea->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    layout->addWidget(pScrollArea);

    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(10, 16, 10, 0);
    m_contentLayout->setSpacing(16);

    pScrollArea->setWidget(m_contentWidget);
    pScrollArea->setWidgetResizable(true);

    m_categoryBtnGroup = new QButtonGroup(this);
    m_categoryBtnGroup->setExclusive(true);
    connect(m_categoryBtnGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
            this, &CategorySideBar::handleCategoryItemToggled);

    loadCategories();
}

bool CategorySideBar::event(QEvent *event)
{
    return QWidget::event(event);
}

void CategorySideBar::handleCategoryItemToggled(QAbstractButton *btn, bool checked)
{
    if (!checked)
        return;

    CategoryItem *item = qobject_cast<CategoryItem *>(btn);
    QString categoryID = item->getCategoryID();

    if (categoryID == m_curCategoryID)
        return;

    emit currentCategoryIndexChanged(m_curCategoryID, categoryID);

    m_curCategoryID = categoryID;
}

void CategorySideBar::loadCategories()
{
    auto cmanager = CategoryManager::instance();
    QList<Category *> categorys = cmanager->getCategorys();

    int idx = 0;
    for (auto category : categorys)
    {
        auto categoryItem = new CategoryItem();
        categoryItem->setAccessibleName(QString("ControlPanelCategory::%1").arg(category->getID()));
        categoryItem->setText(category->getName());
        categoryItem->setIcon(QIcon(category->getIcon()));
        categoryItem->setCategoryID(category->getID());

        categoryItem->setFixedHeight(50);
        categoryItem->setFixedWidth(162);
        categoryItem->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        m_categoryBtnGroup->addButton(categoryItem, idx);
        m_contentLayout->addWidget(categoryItem);

        m_categorysIDMap[category->getID()] = idx;
        idx++;
    }

    auto categorySpaceItem = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_contentLayout->addItem(categorySpaceItem);
}

QString CategorySideBar::getCurrentCateogryID()
{
    return m_curCategoryID;
}

void CategorySideBar::setCurrentCategoryID(const QString &categoryID)
{
    if (categoryID == m_curCategoryID)
        return;

    int idx = m_categorysIDMap[categoryID];
    auto btn = m_categoryBtnGroup->button(idx);
    btn->setChecked(true);
}