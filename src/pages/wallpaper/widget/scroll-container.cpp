#include "scroll-container.h"
#include <QResizeEvent>
#include <QStyleOption>
#include <QPainter>
ScrollContainer::ScrollContainer(QWidget *parent) :
    QWidget(parent)
{

}

void ScrollContainer::resizeEvent(QResizeEvent *e)
{
    emit resized(e->size());
}

void ScrollContainer::paintEvent(QPaintEvent *event)
{
    QStyleOption option;
    option.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);
}
