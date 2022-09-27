#pragma once

#include <kiran-sidebar-item.h>

// 派生于KiranSidebarWidget之中的KiranSidebarItem，重写‘<’操作符号，用于自定义QListWidget::sortItems的排序规则
class ListWidgetSubItem : public KiranSidebarItem
{
public:
    explicit ListWidgetSubItem(QListWidget *view = nullptr, int type = Type);
    explicit ListWidgetSubItem(const QString &text, QListWidget *view = nullptr, int type = Type);
    explicit ListWidgetSubItem(const QIcon &icon, const QString &text,
                               QListWidget *view = nullptr, int type = Type);
    ListWidgetSubItem(const QListWidgetItem &other);
    virtual ~ListWidgetSubItem();

    virtual bool operator<(const QListWidgetItem &other) const;

    int getWeight();
    void setWeight(int weight);
};