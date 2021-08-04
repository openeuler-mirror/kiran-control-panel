/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRANMODULECLASSWIDGET_H
#define KIRANMODULECLASSWIDGET_H

#include <QWidget>
#include "category-listwidget.h"

namespace Ui
{
class CategoryWidget;
}

class QPropertyAnimation;
class CategoryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CategoryWidget(QWidget *parent = 0);
    ~CategoryWidget();

    void init();
    int iconModeWidth();
    void setCurrentCategory(int categoryIndex);

signals:
    void sigCurrentCategoryChanged(int currentIndex,int prevIndex);

private:
    void loadCategory();
    void setIconMode(bool iconMode);
    int textModeWd();
    void drawShadow(QPainter &painter);

protected:
    bool event(QEvent *event) override;

private:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::CategoryWidget *ui;
    bool m_showShadow = false;  //是否有阴影
    int m_shadowWidth = 0;  //阴影宽度
    QPropertyAnimation *m_categorySliderAnimation = nullptr;
};

#endif  // KIRANMODULECLASSWIDGET_H
