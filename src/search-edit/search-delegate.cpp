//
// Created by liuxinhao on 2022/6/16.
//

#include "search-delegate.h"

SearchDelegate::SearchDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

SearchDelegate::~SearchDelegate()
{
}

void SearchDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
}

QSize SearchDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QItemDelegate::sizeHint(option,index);
    size.setHeight(36);
    return size;
}
