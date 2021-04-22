/**
 * @file kiran-list-widget.cpp
 * @brief 用于UI界面提升的列表控件
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "kiran-list-widget.h"


KiranListWidget::KiranListWidget(QWidget *parent)
    :QListWidget(parent)
{

}


QSize KiranListWidget::sizeHint() const
{
    QSize size = QSize(282,-1);
    return size;
}
