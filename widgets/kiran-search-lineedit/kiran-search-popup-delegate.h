/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANSEARCHPOPUPDELEGATE_H
#define KIRANSEARCHPOPUPDELEGATE_H

#include <QStyledItemDelegate>

class StudentListViewModel;
class StudentFrame;

class KiranSearchPopupDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit KiranSearchPopupDelegate(QObject *parent = 0);
    ~KiranSearchPopupDelegate();
    void setSearchText(const QString &text);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QString m_regFindKeyWords;
    void paintKeywordHighlight(QPainter *painter, const QStyleOptionViewItem &itemOption, const QModelIndex &index) const;
    //QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


#endif // KIRANSEARCHPOPUPDELEGATE_H
