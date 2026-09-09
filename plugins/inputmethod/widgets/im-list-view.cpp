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

#include "im-list-view.h"
#include "im-list-item.h"
#include "logging-category.h"

#include <style-palette.h>

#include <QCursor>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPalette>
#include <QPixmap>
#include <QVBoxLayout>

using namespace Kiran;

const QString ImListView::MIME_TYPE = QStringLiteral("application/x-kiran-cpanel-inputmethod-entry");

namespace
{
const int kItemSpacing = 8;
const int kIndicatorHeight = 2;
}  // namespace

ImListView::ImListView(QWidget* parent)
    : QWidget(parent)
{
    initUI();
}

ImListView::~ImListView()
{
}

void ImListView::initUI()
{
    setObjectName("ImListView");
    setAcceptDrops(true);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_itemContainer = new QWidget(this);
    m_itemContainer->setObjectName("widget_items");
    m_itemLayout = new QVBoxLayout(m_itemContainer);
    m_itemLayout->setContentsMargins(0, 0, 0, 0);
    m_itemLayout->setSpacing(kItemSpacing);
    mainLayout->addWidget(m_itemContainer);

    // 拖拽落点指示线，仅拖拽期间可见
    m_dropIndicator = new QWidget(m_itemContainer);
    m_dropIndicator->setObjectName("widget_dropIndicator");
    m_dropIndicator->setFixedHeight(kIndicatorHeight);
    m_dropIndicator->setAutoFillBackground(true);
    QPalette indicatorPalette = m_dropIndicator->palette();
    indicatorPalette.setColor(QPalette::Window,
                              StylePalette::instance()->color(StylePalette::Checked,
                                                               StylePalette::Bare,
                                                               StylePalette::Foreground));
    m_dropIndicator->setPalette(indicatorPalette);
    m_dropIndicator->hide();
}

void ImListView::setEntries(const QVector<ImEntry>& entries)
{
    m_entries = entries;

    if (m_dragging)
    {
        // 拖拽进行中（drag->exec 嵌套事件循环内），此刻重建会销毁正在拖拽的条目，
        // 而 QDrag 仍持有其 pixmap/指针。仅记录数据，待拖拽结束后补一次重建。
        m_pendingRebuild = true;
        return;
    }

    rebuildItems();
}

void ImListView::clearItems()
{
    for (auto* item : m_items)
    {
        m_itemLayout->removeWidget(item);
        item->deleteLater();
    }
    m_items.clear();
}

ImListItem* ImListView::createItem(const ImEntry& entry)
{
    auto* item = new ImListItem(entry, m_itemContainer);
    connect(item, &ImListItem::dragStarted, this, &ImListView::onItemDragStarted);
    connect(item, &ImListItem::deleteClicked, this, &ImListView::onItemDeleteClicked);
    return item;
}

void ImListView::rebuildItems()
{
    clearItems();

    for (const auto& entry : m_entries)
    {
        auto* item = createItem(entry);
        m_itemLayout->addWidget(item);
        m_items.append(item);
    }

    updateItemsState();
}

void ImListView::updateItemsState()
{
    const int count = m_items.size();
    const QString tips = tr("Drag to reorder.");

    for (int i = 0; i < count; i++)
    {
        ImListItem* item = m_items.at(i);
        // 仅剩一项时禁止删除，避免写回空的输入法组
        item->setDeletable(count > 1);
        item->setToolTip(tips);
    }
}

int ImListView::indexForPosition(int y) const
{
    if (m_items.isEmpty())
    {
        return 0;
    }

    for (int i = 0; i < m_items.size(); i++)
    {
        ImListItem* item = m_items.at(i);
        int top = item->mapTo(const_cast<ImListView*>(this), QPoint(0, 0)).y();
        int center = top + item->height() / 2;
        if (y < center)
        {
            return i;
        }
    }
    return m_items.size();
}

void ImListView::onItemDragStarted(ImListItem* item)
{
    int index = m_items.indexOf(item);
    if (index < 0 || m_items.size() < 2)
    {
        return;
    }

    auto* mimeData = new QMimeData();
    mimeData->setData(MIME_TYPE, QByteArray::number(index));

    auto* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    QPixmap pixmap = item->grab();
    drag->setPixmap(pixmap);
    QPoint hotSpot = item->mapFromGlobal(QCursor::pos());
    if (item->rect().contains(hotSpot))
    {
        drag->setHotSpot(hotSpot);
    }

    // drag->exec 会进入嵌套事件循环，拖拽全程都在其中运行。
    // 期间若外部变更触发 setEntries → rebuildItems，正在被拖拽的条目会被销毁，
    // 而 QDrag 仍持有其指针 → 悬空访问。故拖拽期挂起重建，结束后补一次。
    m_dragging = true;
    drag->exec(Qt::MoveAction);
    m_dragging = false;

    m_dropIndicator->hide();

    if (m_pendingRebuild)
    {
        m_pendingRebuild = false;
        rebuildItems();
    }
}

void ImListView::onItemDeleteClicked(const ImEntry& entry)
{
    if (m_entries.size() <= 1)
    {
        qCWarning(qLcInputMethod) << "refuse to remove the last input method:" << entry.uniqueName;
        return;
    }

    int index = -1;
    for (int i = 0; i < m_entries.size(); i++)
    {
        if (m_entries.at(i).uniqueName == entry.uniqueName)
        {
            index = i;
            break;
        }
    }
    if (index < 0)
    {
        return;
    }

    const QString uniqueName = m_entries.at(index).uniqueName;
    m_entries.remove(index);
    rebuildItems();

    emit entryRemoved(uniqueName);
}

void ImListView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->source() == this && event->mimeData()->hasFormat(MIME_TYPE))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
        return;
    }
    QWidget::dragEnterEvent(event);
}

void ImListView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->source() != this || !event->mimeData()->hasFormat(MIME_TYPE))
    {
        QWidget::dragMoveEvent(event);
        return;
    }

    int target = indexForPosition(event->pos().y());

    // 计算指示线位置（容器坐标系）
    int y = 0;
    if (m_items.isEmpty())
    {
        y = 0;
    }
    else if (target >= m_items.size())
    {
        ImListItem* last = m_items.last();
        y = last->geometry().bottom() + kItemSpacing / 2;
    }
    else
    {
        ImListItem* item = m_items.at(target);
        y = item->geometry().top() - kItemSpacing / 2;
    }

    m_dropIndicator->setFixedWidth(m_itemContainer->width());
    m_dropIndicator->move(0, qMax(0, y));
    m_dropIndicator->show();
    m_dropIndicator->raise();

    event->setDropAction(Qt::MoveAction);
    event->accept();
}

void ImListView::dragLeaveEvent(QDragLeaveEvent* event)
{
    m_dropIndicator->hide();
    QWidget::dragLeaveEvent(event);
}

void ImListView::dropEvent(QDropEvent* event)
{
    m_dropIndicator->hide();

    if (event->source() != this || !event->mimeData()->hasFormat(MIME_TYPE))
    {
        QWidget::dropEvent(event);
        return;
    }

    bool ok = false;
    int from = event->mimeData()->data(MIME_TYPE).toInt(&ok);
    if (!ok || from < 0 || from >= m_entries.size())
    {
        event->ignore();
        return;
    }

    int to = indexForPosition(event->pos().y());
    // 落点在自身前后时顺序不变
    if (to == from || to == from + 1)
    {
        event->acceptProposedAction();
        return;
    }

    if (to > from)
    {
        to--;
    }
    to = qBound(0, to, m_entries.size() - 1);

    ImEntry moved = m_entries.at(from);
    m_entries.remove(from);
    m_entries.insert(to, moved);

    rebuildItems();

    event->setDropAction(Qt::MoveAction);
    event->accept();

    emit orderChanged(m_entries);
}
