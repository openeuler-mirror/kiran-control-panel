//
// Created by liuxinhao on 2022/5/26.
//

#ifndef KCP_CATEGORY_CATEGORY_WIDGET_H
#define KCP_CATEGORY_CATEGORY_WIDGET_H

#include <QWidget>
#include <QPropertyAnimation>

class QButtonGroup;
class QAbstractButton;
class QLayout;
class QFrame;
class CategoryWidget : public QWidget
{
    Q_OBJECT
public:
    CategoryWidget(QWidget* parent = nullptr);
    ~CategoryWidget();

    int getCurrentCateogryIdx();
    void setCurrentCategoryIdx(int idx);

    const static int reduce_width;
    const static int expand_width;
signals:
    void currentCategoryIndexChanged(int curIdx,int prevIdx);

private:
    void init();
    void loadCategories();
    void expand();
    void reduce();

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void handleCategoryItemToggled(QAbstractButton* btn,bool checked);

protected:
    bool event(QEvent *event) override;

private:
    QButtonGroup* m_categoryBtnGroup;
    QPropertyAnimation m_propertyAnimation;
    QWidget* m_contentWidget = nullptr;
    QLayout* m_contentLayout = nullptr;
    QFrame* m_splitLine = nullptr;
    int m_currentCategoryIdx = -1;
    bool m_isExpaned = false;
};


#endif //KCP_CATEGORY_CATEGORY_WIDGET_H
