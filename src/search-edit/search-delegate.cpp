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