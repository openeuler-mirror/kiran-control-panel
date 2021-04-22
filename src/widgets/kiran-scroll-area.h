/**
 * @file kiran-scroll-area.h
 * @brief 用于UI界面提升的滑动条
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef KIRANSCROLLAREA_H
#define KIRANSCROLLAREA_H

#include <QScrollArea>

class KiranScrollArea : public QScrollArea
{
public:
    KiranScrollArea(QWidget* parent=nullptr);
    ~KiranScrollArea()=default;

    virtual QSize sizeHint() const override;
};

#endif // KIRANSCROLLAREA_H
