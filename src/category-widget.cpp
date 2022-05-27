//
// Created by liuxinhao on 2022/5/26.
//

#include "category-widget.h"
#include "category-item.h"
#include "plugin-manager.h"

#include <kiran-palette.h>

#include <QButtonGroup>
#include <QEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>
#include <QBoxLayout>

const int CategoryWidget::reduce_width = 88;
const int CategoryWidget::expand_width = 244;

CategoryWidget::CategoryWidget(QWidget *parent) : QWidget(parent)
{
    init();
}

CategoryWidget::~CategoryWidget()
{
}

void CategoryWidget::init()
{
    //感知鼠标悬浮
    setAttribute(Qt::WA_Hover);
    parentWidget()->installEventFilter(this);

    //初始化动画相关参数
    m_propertyAnimation.setTargetObject(this);
    m_propertyAnimation.setPropertyName("geometry");

    //布局
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    //滚动区域
    auto pScrollArea = new QScrollArea(this);
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
    layout->addWidget(m_splitLine);

    m_categoryBtnGroup = new QButtonGroup(this);
    m_categoryBtnGroup->setExclusive(true);
    connect(m_categoryBtnGroup,QOverload<QAbstractButton*,bool>::of(&QButtonGroup::buttonToggled),
            this,&CategoryWidget::handleCategoryItemToggled);

    loadCategories();
}

bool CategoryWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parent() && event->type() == QEvent::Resize)
    {
        auto resizeEvent = dynamic_cast<QResizeEvent *>(event);
        resize(width(), resizeEvent->size().height());
    }
    return QObject::eventFilter(watched, event);
}

bool CategoryWidget::event(QEvent *event)
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

void CategoryWidget::expand()
{
    //m_contentWidget->setBackgroundRole(QPalette::Button);
    m_propertyAnimation.setStartValue(QRect(0, 0, reduce_width, this->height()));
    m_propertyAnimation.setEndValue(QRect(0, 0, expand_width, this->height()));
    m_propertyAnimation.setEasingCurve(QEasingCurve::InCubic);
    m_propertyAnimation.start();
}

void CategoryWidget::reduce()
{
    //m_propertyAnimation.setStartValue(QRect(0, 0, expand_width, this->height()));
    m_propertyAnimation.setEndValue(QRect(0, 0, reduce_width, this->height()));
    m_propertyAnimation.setEasingCurve(QEasingCurve::OutCubic);
    m_propertyAnimation.start();
}

void CategoryWidget::handleCategoryItemToggled(QAbstractButton *btn, bool checked)
{
    if( !checked )
        return;

    int checkedIdx = m_categoryBtnGroup->id(btn);
    if( checkedIdx == m_currentCategoryIdx )
        return;

    int prevIdx = m_currentCategoryIdx;
    m_currentCategoryIdx = checkedIdx;

    emit currentCategoryIndexChanged(m_currentCategoryIdx,prevIdx);
}

void CategoryWidget::loadCategories()
{
    auto categories = CPanelPluginManager::getInstance()->getCategorys();
    for( int i=0;i<categories.count();i++)
    {
        auto category = categories.at(i);
        auto categoryInfo = category->getCategoryDesktopInfo();
        auto categoryItem = new CategoryItem();
        categoryItem->setText(categoryInfo.name);
        categoryItem->setIcon(QIcon(categoryInfo.icon));
        categoryItem->setFixedHeight(50);
        categoryItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_categoryBtnGroup->addButton(categoryItem, i);
        m_contentLayout->addWidget(categoryItem);
    }

    auto categorySpaceItem = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_contentLayout->addItem(categorySpaceItem);
}

int CategoryWidget::getCurrentCateogryIdx()
{
    return m_currentCategoryIdx;
}

void CategoryWidget::setCurrentCategoryIdx(int idx)
{
    if( idx == m_currentCategoryIdx )
        return;
    auto btn = m_categoryBtnGroup->button(idx);
    btn->setChecked(true);
}
