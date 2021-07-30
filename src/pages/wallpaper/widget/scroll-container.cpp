#include "scroll-container.h"
#include <QResizeEvent>
ScrollContainer::ScrollContainer(QWidget *parent) :
    QWidget(parent)
{

}

void ScrollContainer::resizeEvent(QResizeEvent *e)
{
    emit resized(e->size());
}
