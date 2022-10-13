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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once

#include <kiran-sidebar-item.h>

// 派生于KiranSidebarWidget之中的KiranSidebarItem，重写‘<’操作符号，用于自定义QListWidget::sortItems的排序规则
class ListWidgetSubItem : public KiranSidebarItem
{
public:
    explicit ListWidgetSubItem(QListWidget *view = nullptr, int type = Type);
    explicit ListWidgetSubItem(const QString &text, QListWidget *view = nullptr, int type = Type);
    explicit ListWidgetSubItem(const QIcon &icon, const QString &text,
                               QListWidget *view = nullptr, int type = Type);
    ListWidgetSubItem(const QListWidgetItem &other);
    virtual ~ListWidgetSubItem();

    virtual bool operator<(const QListWidgetItem &other) const;

    int getWeight();
    void setWeight(int weight);
};