/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRANMODULECLASSLISTWIDGET_H
#define KIRANMODULECLASSLISTWIDGET_H

#include "category-listwidget-item.h"

#include <QButtonGroup>
#include <QListWidget>

//TODO: 重写该部分
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
