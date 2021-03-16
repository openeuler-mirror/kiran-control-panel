#include "kiran-list-widget.h"


KiranListWidget::KiranListWidget(QWidget *parent)
    :QListWidget(parent)
{

}


QSize KiranListWidget::sizeHint() const
{

//    QSize areaSizeHint = widget()->sizeHint();
    QSize size = QSize(288,-1);
    //areaSizeHint+=QSize(verticalScrollBar()->width(),horizontalScrollBar()->height());
    return size;
}
