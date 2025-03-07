/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#ifndef USERSCONTAINER_H
#define USERSCONTAINER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QBoxLayout;
QT_END_NAMESPACE

class QScrollArea;
class UserListItem;
class UsersContainer : public QWidget
{
    Q_OBJECT
public:
    UsersContainer(QWidget* parent = nullptr);
    virtual ~UsersContainer();

    /// @brief 向布局中添加一个用户条目
    /// @param widget 用户条目
    void addItem(UserListItem* widget);

    /// @brief 设置用户条目的可见性
    /// @param name 用户条目的name
    /// @param visible 是否可见
    void setItemVisible(const QString& name, bool visible);

    /// @brief 获取选中的用户条目
    /// @return 选中的用户条目
    /// @note 获取到的用户条目是选中状态的
    QList<UserListItem*> getSelectedItems();

    /// @brief 清空用户条目
    void clear();

private:
    void init();
    void adjustSizeToItmes();

private:
    QBoxLayout* m_mainLayout;
    QBoxLayout* m_containerLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
};
#endif  // USERSCONTAINER_H
