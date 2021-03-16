#include "kiran-scroll-area.h"
#include <QScrollBar>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>

KiranScrollArea::KiranScrollArea(QWidget* parent)
    :QScrollArea(parent)
{

}

QSize KiranScrollArea::sizeHint() const
{
    if(!widget()){
        return QScrollArea::sizeHint();
    }

    QSize areaSizeHint = QSize(636,690);
    areaSizeHint+=QSize(verticalScrollBar()->width(),horizontalScrollBar()->height());

    return areaSizeHint;
}
