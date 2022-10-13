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

#include <style-palette.h>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>

const int shadow_width = 10;
const int CategorySideBar::reduce_width = 88;
const int CategorySideBar::expand_width = 244;

CategorySideBar::CategorySideBar(QWidget *parent) : QWidget(parent)
{
    init();
}

CategorySideBar::~CategorySideBar()
{
}

void CategorySideBar::init()
{
    setAccessibleName("ControlPanelCategory");

    //初始化阴影
    QColor shadowColor(0, 0, 0, 255 * 0.9);
    m_dropShadowEffect = new QGraphicsDropShadowEffect(this);
    m_dropShadowEffect->setBlurRadius(20);
    m_dropShadowEffect->setOffset(-4, 0);
    m_dropShadowEffect->setColor(shadowColor);
    m_dropShadowEffect->setEnabled(false);
    this->setGraphicsEffect(m_dropShadowEffect);

    //感知鼠标悬浮
    setAttribute(Qt::WA_Hover);
    setAttribute(Qt::WA_TranslucentBackground);
    parentWidget()->installEventFilter(this);

    //初始化动画相关参数
    m_propertyAnimation.setTargetObject(this);
    m_propertyAnimation.setPropertyName("geometry");

    //布局
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, shadow_width, 0);

    //滚动区域
    auto pScrollArea = new QScrollArea(this);
    pScrollArea->setAutoFillBackground(true);
    pScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pScrollArea->setFrameStyle(QFrame::NoFrame);
    layout->addWidget(pScrollArea);

    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(12, 6, 12, 0);
    m_contentLayout->setSpacing(6);

    pScrollArea->setWidget(m_contentWidget);
    pScrollArea->setWidgetResizable(true);

    //分隔线条
    m_splitLine = new QFrame(this);
    m_splitLine->setFrameShape(QFrame::VLine);
    m_splitLine->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_splitLine->setFixedWidth(1);
    layout->addWidget(m_splitLine);

    m_categoryBtnGroup = new QButtonGroup(this);
    m_categoryBtnGroup->setExclusive(true);
    connect(m_categoryBtnGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
            this, &CategorySideBar::handleCategoryItemToggled);

    loadCategories();
    resize(reduce_width + shadow_width, 0);
}

bool CategorySideBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parent() && event->type() == QEvent::Resize)
    {
        auto resizeEvent = dynamic_cast<QResizeEvent *>(event);
        resize(width(), resizeEvent->size().height());
    }
    return QObject::eventFilter(watched, event);
}

bool CategorySideBar::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::HoverEnter:
        expand();
        break;
    case QEvent::HoverLeave:
        reduce();
        break;
    }
    return QWidget::event(event);
}

void CategorySideBar::expand()
{
    m_isExpaned = true;
    m_dropShadowEffect->setEnabled(true);
    m_propertyAnimation.setStartValue(QRect(0, 0, reduce_width + shadow_width, this->height()));
    m_propertyAnimation.setEndValue(QRect(0, 0, expand_width + shadow_width, this->height()));
    m_propertyAnimation.setEasingCurve(QEasingCurve::InCubic);
    m_propertyAnimation.start();
}

void CategorySideBar::reduce()
{
    m_isExpaned = false;
    m_dropShadowEffect->setEnabled(false);
    m_propertyAnimation.setStartValue(QRect(0, 0, expand_width + shadow_width, this->height()));
    m_propertyAnimation.setEndValue(QRect(0, 0, reduce_width + shadow_width, this->height()));
    m_propertyAnimation.setEasingCurve(QEasingCurve::OutCubic);
    m_propertyAnimation.start();
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
        categoryItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

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

void CategorySideBar::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QWidget::paintEvent(event);
}