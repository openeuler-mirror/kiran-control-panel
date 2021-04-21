/**
 * @file kiran-list-widget.h
 * @brief 用于UI界面提升的列表控件
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef KIRANLISTWIDGET_H
#define KIRANLISTWIDGET_H

#include <QListWidget>

class KiranListWidget :public QListWidget
{
public:
    KiranListWidget(QWidget* parent=nullptr);
    ~KiranListWidget() = default;

    virtual QSize sizeHint() const override;
};

#endif // KIRANLISTWIDGET_H
