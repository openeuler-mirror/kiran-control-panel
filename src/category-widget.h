/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
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
