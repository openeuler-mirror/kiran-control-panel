#include "users-container.h"
#include <QBoxLayout>
#include <QScrollArea>

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

void UsersContainer::addFeatureItem(QWidget* widget)
{
    m_containerLayout->addWidget(widget, 0);
    adjustSizeToItmes();
}

QList<QWidget*> UsersContainer::getAllFeatureItem()
{
    QList<QWidget*> result;
    QLayoutItem* child = nullptr;
    while ((child = m_containerLayout->takeAt(0)) != 0)
    {
        if (child->widget())
        {
            result.append(child->widget());
        }
    }
    return result;
}

void UsersContainer::clear()
{
    deleteAllItemOfLayout(m_containerLayout);
}

void UsersContainer::init()
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

void UsersContainer::adjustSizeToItmes()
{
    auto mainMargin = m_mainLayout->contentsMargins();
    auto contentMargin = m_containerLayout->contentsMargins();
    auto contentSpacing = m_containerLayout->spacing();

    int height = mainMargin.top() + mainMargin.bottom() + contentMargin.top() + contentMargin.bottom();
    height += contentSpacing * (m_containerLayout->count() - 1);
    height += 36 * m_containerLayout->count();
    setMaximumHeight(height > 0 ? height : 0);
}
