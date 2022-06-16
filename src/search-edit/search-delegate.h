//
// Created by liuxinhao on 2022/6/16.
//

#ifndef KIRAN_CONTROL_PANEL_SEARCH_DELEGATE_H
#define KIRAN_CONTROL_PANEL_SEARCH_DELEGATE_H

#include <QItemDelegate>

class SearchDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit SearchDelegate(QObject* parent = nullptr);
    ~SearchDelegate();

public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif  // KIRAN_CONTROL_PANEL_SEARCH_DELEGATE_H
