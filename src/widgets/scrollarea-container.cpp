#include "scrollarea-container.h"

#include <QStyleOption>
#include <QPainter>

ScrollAreaContainer::ScrollAreaContainer (QWidget *parent) : QWidget(parent)
{

}

QSize ScrollAreaContainer::sizeHint () const
{
    return minimumSize();
}

void ScrollAreaContainer::paintEvent (QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}
