#ifndef LISTWIDGETWRAPPER_H
#define LISTWIDGETWRAPPER_H

#include <QListWidget>
#include <QItemSelectionModel>

class ListWidgetControl : public QListWidget
{
public:
    ListWidgetControl(QWidget* parent=nullptr);

protected:
    virtual QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                                 const QEvent *event = Q_NULLPTR) const override;
};

#endif // LISTWIDGETWRAPPER_H
