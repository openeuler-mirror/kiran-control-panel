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

#ifndef IM_LIST_VIEW_H
#define IM_LIST_VIEW_H

#include <QList>
#include <QString>
#include <QVector>
#include <QWidget>

#include "im-entry.h"

class QVBoxLayout;
class ImListItem;

/// 已启用输入法列表
/// - 支持鼠标拖拽上下调整顺序（QDrag + 自定义 mime type）
/// - 支持删除条目
/// - 列表语义说明（首项非激活态/第二项默认）由页面层在滚动区外渲染
class ImListView : public QWidget
{
    Q_OBJECT
public:
    explicit ImListView(QWidget* parent = nullptr);
    ~ImListView() override;

    /// 全量重建列表
    /// 拖拽进行中时只记录数据，待拖拽结束后补一次重建（见 setEntries 实现）
    void setEntries(const QVector<ImEntry>& entries);
    QVector<ImEntry> entries() const { return m_entries; }

    /// 是否正在拖拽（drag->exec 嵌套事件循环内）
    bool isDragging() const { return m_dragging; }

signals:
    /// 拖拽排序完成，参数为排序后的完整列表
    void orderChanged(const QVector<ImEntry>& entries);
    /// 条目被删除
    void entryRemoved(const QString& uniqueName);
    /// 请求添加输入法（保留给外部标题栏按钮或兼容调用）
    void addRequested();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void initUI();
    void rebuildItems();
    void clearItems();
    ImListItem* createItem(const ImEntry& entry);
    /// 刷新徽章、删除按钮可用性、tooltip
    void updateItemsState();
    /// 依据 y 坐标计算插入位置（0 ~ count）
    int indexForPosition(int y) const;

private slots:
    void onItemDragStarted(ImListItem* item);
    void onItemDeleteClicked(const ImEntry& entry);

private:
    QVBoxLayout* m_itemLayout = nullptr;
    QWidget* m_itemContainer = nullptr;
    /// 拖拽落点指示线容器（简单用一个细条 widget 表示）
    QWidget* m_dropIndicator = nullptr;

    QVector<ImEntry> m_entries;
    QList<ImListItem*> m_items;

    /// 拖拽进行中（drag->exec 嵌套事件循环内），此期间禁止重建条目
    bool m_dragging = false;
    /// 拖拽期间收到过 setEntries，待拖拽结束后补一次重建
    bool m_pendingRebuild = false;

    static const QString MIME_TYPE;
};

#endif  // IM_LIST_VIEW_H
