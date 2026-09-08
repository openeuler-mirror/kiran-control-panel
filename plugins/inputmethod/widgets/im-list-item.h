/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
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

#ifndef IM_LIST_ITEM_H
#define IM_LIST_ITEM_H

#include <QPoint>
#include <QRect>
#include <QWidget>

#include "im-entry.h"

/// 输入法列表条目（自绘）
/// 布局：图标(32x32) | 显示名 | 删除按钮(悬浮显示)
/// 拖拽由所属的 ImListView 统一发起，此处只负责识别拖拽起始并发出信号
class ImListItem : public QWidget
{
    Q_OBJECT
public:
    explicit ImListItem(const ImEntry& entry, QWidget* parent = nullptr);
    ~ImListItem() override;

    const ImEntry& entry() const { return m_entry; }
    QString uniqueName() const { return m_entry.uniqueName; }
    void setEntry(const ImEntry& entry);

    /// 是否允许删除（列表仅剩一项时禁止删除，避免写回空列表）
    void setDeletable(bool deletable);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void deleteClicked(const ImEntry& entry);
    void dragStarted(ImListItem* item);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void initUI();
    QString primaryText() const;
    QRect deleteButtonRect() const;

private:
    ImEntry m_entry;
    bool m_deletable = true;
    bool m_hovered = false;
    bool m_deleteHovered = false;
    bool m_pressed = false;
    bool m_pressedOnDelete = false;
    QPoint m_pressPos;
};

#endif  // IM_LIST_ITEM_H
