/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */

#pragma once
#include <QWidget>

namespace Ui
{
class ListExpansionSpace;
}

class ListExpansionSpace : public QWidget
{
    Q_OBJECT

public:
    explicit ListExpansionSpace(QWidget *parent = nullptr);
    ~ListExpansionSpace() override;

signals:
    void addedListWidgetItem();
    void removedListWidgetItem();

public slots:
    /**
     * @brief 添加控件到列表
     * @param widget
     */
    void addListExpansionSpaceItem(QWidget *widget);

    /**
      * @brief 移除当前选中的列表条目
      */
    void removeListExpansionSpaceCurrentItem();

    /**
     * @brief 根据索引删除列表条目
     * @param index
     */
    void removeListExpansionSpaceItem(int index);

private:
    void init();

private:
    Ui::ListExpansionSpace *ui;
};
