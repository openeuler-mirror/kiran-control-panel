#ifndef KIRANMODECLASSDELEGATE_H
#define KIRANMODECLASSDELEGATE_H

#include <QStyledItemDelegate>

//! [0]
class KiranModeClassDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    KiranModeClassDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

};

#endif // KIRANMODECLASSDELEGATE_H
