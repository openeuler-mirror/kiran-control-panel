/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULECLASSLISTWIDGET_H
#define KIRANMODULECLASSLISTWIDGET_H

#include "category-listwidget-item.h"

#include <QButtonGroup>
#include <QListWidget>

class CategoryListWidget : public QListWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isIconMode READ isIconMode WRITE setIconMode)
public:
    explicit CategoryListWidget(QWidget *parent = nullptr);
    ~CategoryListWidget() override = default;

    bool isIconMode() const;

private:
    void init();
    void initUI();

public slots:
    void setIconMode(const bool &iconMode = true);
    void setTextShow(const bool &showText);

protected:
    void resizeEvent(QResizeEvent *event) override;
    /* 重新实现selectionCommand，以自定义选择行为 */
    virtual QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                                 const QEvent *event = Q_NULLPTR) const override;

private:
    QString styleSheetStr();
    QString styleExpandSheetStr();

private:
    bool m_isIconMode = true;
};

#endif  // KIRANMODULECLASSLISTWIDGET_H
