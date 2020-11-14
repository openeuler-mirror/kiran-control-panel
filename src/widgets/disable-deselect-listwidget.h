#ifndef LISTWIDGETWRAPPER_H
#define LISTWIDGETWRAPPER_H

#include <QListWidget>
#include <QItemSelectionModel>

/**
 * @brief 禁用按住ctrl进行反选，禁用用户按住鼠标在listwidget上移动进行切换选中的情况
 */
class DisableDeselectListWidget : public QListWidget
{
public:
    DisableDeselectListWidget(QWidget* parent=nullptr);
protected:
    virtual QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                                 const QEvent *event = Q_NULLPTR) const override;
};


#endif // LISTWIDGETWRAPPER_H
