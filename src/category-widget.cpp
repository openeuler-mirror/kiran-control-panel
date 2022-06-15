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
#include <QGraphicsDropShadowEffect>

const int shadow_width=10;
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
    //初始化阴影
    QColor shadowColor(0,0,0,255*0.9);
    m_dropShadowEffect = new QGraphicsDropShadowEffect(this);
    m_dropShadowEffect->setBlurRadius(20);
    m_dropShadowEffect->setOffset(-4,0);
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
    layout->setContentsMargins(0,0,shadow_width,0);

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
    m_splitLine->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    m_splitLine->setFixedWidth(1);
    layout->addWidget(m_splitLine);

    m_categoryBtnGroup = new QButtonGroup(this);
    m_categoryBtnGroup->setExclusive(true);
    connect(m_categoryBtnGroup,QOverload<QAbstractButton*,bool>::of(&QButtonGroup::buttonToggled),
            this,&CategoryWidget::handleCategoryItemToggled);

    loadCategories();
    resize(reduce_width+shadow_width,0);
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
    m_isExpaned=true;
    m_dropShadowEffect->setEnabled(true);
    m_propertyAnimation.setStartValue(QRect(0, 0, reduce_width+shadow_width, this->height()));
    m_propertyAnimation.setEndValue(QRect(0, 0, expand_width+shadow_width, this->height()));
    m_propertyAnimation.setEasingCurve(QEasingCurve::InCubic);
    m_propertyAnimation.start();
}

void CategoryWidget::reduce()
{
    m_isExpaned=false;
    m_dropShadowEffect->setEnabled(false);
    m_propertyAnimation.setStartValue(QRect(0, 0, expand_width+shadow_width, this->height()));
    m_propertyAnimation.setEndValue(QRect(0, 0, reduce_width+shadow_width, this->height()));
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
    auto categories = PluginManager::getInstance()->getCategorys();
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

void CategoryWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QWidget::paintEvent(event);
}