//
// Created by liuxinhao on 2022/5/27.
//

#ifndef KCP_CATEGORY_CATEGORY_ITEM_H
#define KCP_CATEGORY_CATEGORY_ITEM_H

#include <QWidget>
#include <QAbstractButton>

class CategoryItem : public QAbstractButton
{
    Q_OBJECT
public:
    CategoryItem(QWidget* parent = nullptr);
    ~CategoryItem();

    void setCategoryID(const QString& id);
    QString getCategoryID();

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    QString m_categoryID;
};


#endif //KCP_CATEGORY_CATEGORY_ITEM_H
