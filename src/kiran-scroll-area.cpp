/**
 * @file kiran-scroll-area.cpp
 * @brief 用于UI界面提升的滑动条
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "kiran-scroll-area.h"
#include <QScrollBar>
#include <QDesktopWidget>
#include <QApplication>

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
