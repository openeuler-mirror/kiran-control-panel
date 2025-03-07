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

#pragma once

#include <kiran-frame/kiran-frame.h>
#include <QVariant>

class KiranLabel;
class QPushButton;
class UserListItem : public KiranFrame
{
    Q_OBJECT
public:
    explicit UserListItem(QWidget* parent = nullptr);
    ~UserListItem() = default;

    /// @brief 设置用户条目名称
    /// @param name 用户条目名称
    void setName(const QString& name);

    /// @brief 获取用户条目名称
    /// @return 用户条目名称
    QString name();

    /// @brief 设置右侧按钮图标
    /// @param icon 需要设置的图标
    void setRightBtnIcon(const QIcon& icon);

    /// @brief 设置右侧按钮是否可见
    /// @param visible 是否可见
    /// @note 设置为false时，按钮不可见
    void setRightBtnVisible(bool visible);

    /// @brief 设置条目是否可点击
    /// @param clickable 是否可点击
    void setClickable(bool clickable);

    /// @brief 获取条目是否被选中
    /// @return 条目是否被选中
    bool isSelected();

signals:
    void rightButtonClicked();

private:
    void initUI();
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QString m_name;

    QPushButton* m_rightButton;
    KiranLabel* m_label;

    bool m_clickable = false;
    bool m_clicked = false;
    bool m_selected = false;
};