#include "list-widget-subitem.h"

#define ROLE_SORT_WEIGHT Qt::UserRole + 669

ListWidgetSubItem::ListWidgetSubItem(QListWidget *view, int type)
    : KiranSidebarItem(view, type)
{
}

ListWidgetSubItem::ListWidgetSubItem(const QString &text, QListWidget *view, int type)
    : KiranSidebarItem(text, view, type)
{
}

ListWidgetSubItem::ListWidgetSubItem(const QIcon &icon, const QString &text, QListWidget *view, int type)
    : KiranSidebarItem(icon, text, view, type)
{
}

ListWidgetSubItem::ListWidgetSubItem(const QListWidgetItem &other)
    : KiranSidebarItem(other)
{
}

ListWidgetSubItem::~ListWidgetSubItem()
{
}

bool ListWidgetSubItem::operator<(const QListWidgetItem &other) const
{
    bool toInt = false;

    int weight = data(ROLE_SORT_WEIGHT).toInt(&toInt);
    if (!toInt)
        weight = 0;

    int otherWeight = other.data(ROLE_SORT_WEIGHT).toInt(&toInt);
    if (!toInt)
        otherWeight = 0;

    return weight < otherWeight;
}

int ListWidgetSubItem::getWeight()
{
    QVariant var = data(ROLE_SORT_WEIGHT);
    bool toInt = false;
    int weight = var.toInt(&toInt);
    return toInt ? weight : 0;
}

void ListWidgetSubItem::setWeight(int weight)
{
    setData(ROLE_SORT_WEIGHT, QVariant(weight));
}
