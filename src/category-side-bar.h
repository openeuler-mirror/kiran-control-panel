//
// Created by liuxinhao on 2022/5/26.
//

#ifndef KCP_CATEGORY_CATEGORY_WIDGET_H
#define KCP_CATEGORY_CATEGORY_WIDGET_H

#include <QPropertyAnimation>
#include <QWidget>

class QButtonGroup;
class QAbstractButton;
class QLayout;
class QFrame;
class QGraphicsDropShadowEffect;
class CategorySideBar : public QWidget
{
    Q_OBJECT
public:
    CategorySideBar(QWidget* parent = nullptr);
    ~CategorySideBar();

    QString getCurrentCateogryID();
    void setCurrentCategoryID(const QString& categoryID);
    const static int reduce_width;
    const static int expand_width;

signals:
    void currentCategoryIndexChanged(const QString& prev, const QString& cur);

private:
    void init();
    void loadCategories();
    void expand();
    void reduce();

public:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void handleCategoryItemToggled(QAbstractButton* btn, bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;

private:
    QButtonGroup* m_categoryBtnGroup;
    QPropertyAnimation m_propertyAnimation;
    QWidget* m_contentWidget = nullptr;
    QLayout* m_contentLayout = nullptr;
    QFrame* m_splitLine = nullptr;
    QGraphicsDropShadowEffect* m_dropShadowEffect = nullptr;

    QString m_curCategoryID;
    bool m_isExpaned = false;

    /// @brief QMap<Category ID,Categorys Button Group ID>
    QMap<QString, int> m_categorysIDMap;
};

#endif  // KCP_CATEGORY_CATEGORY_WIDGET_H
