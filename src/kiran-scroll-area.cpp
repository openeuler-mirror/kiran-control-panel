#include "kiran-scroll-area.h"
#include <QScrollBar>

KiranScrollArea::KiranScrollArea(QWidget* parent)
    :QScrollArea(parent)
{

}

QSize KiranScrollArea::sizeHint() const
{
    if(!widget()){
        return QScrollArea::sizeHint();
    }
//    QSize areaSizeHint = widget()->sizeHint();
    QSize areaSizeHint = QSize(636,670);
    areaSizeHint+=QSize(verticalScrollBar()->width(),horizontalScrollBar()->height());
    return areaSizeHint;
}
