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

#include "adjusting-scroll-area.h"

#include <QEvent>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

AdjustingScrollArea::AdjustingScrollArea(QWidget* parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);
    // 保留非零的最小视口高度。若设为 Ignored，尾部 stretch 会吃掉全部
    // 高度，导致滚动区被完全压扁。
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    m_contentWidget = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    setWidget(m_contentWidget);

    m_contentWidget->installEventFilter(this);
    viewport()->installEventFilter(this);
}

QWidget* AdjustingScrollArea::contentWidget() const
{
    return m_contentWidget;
}

QVBoxLayout* AdjustingScrollArea::contentLayout() const
{
    return m_contentLayout;
}

int AdjustingScrollArea::maximumContentHeight() const
{
    return m_maximumContentHeight;
}

void AdjustingScrollArea::setMaximumContentHeight(int height)
{
    height = qMax(0, height);
    if (m_maximumContentHeight == height)
    {
        // 值未变时早退，配合 m_geometryUpdatePending 避免重排反馈环。
        return;
    }

    m_maximumContentHeight = height;
    // 这是布局约束而非固定尺寸：父布局仍可分配更小的高度，
    // 此时由 QScrollArea 提供内部滚动。
    setMaximumHeight(height > 0 ? height : QWIDGETSIZE_MAX);
    updateGeometry();
}

QSize AdjustingScrollArea::sizeHint() const
{
    // 与 QScrollArea 默认 sizeHint 相比，本实现额外做了三件事：
    //   1) 以内容 widget 的 sizeHint 为基准，叠加边框宽度；
    //   2) 与 KDE 的 KAdjustingScrollArea 一致，期望几何中预留滚动条占位，
    //      避免内容越过最大高度上限时宽度发生跳变；
    //   3) 整体期望高度被 maximumContentHeight 截断，形成"跟随内容到上限"
    //      的语义：内容少时等于内容高度，内容多时被上限截断。
    QSize hint = QScrollArea::sizeHint();
    if (!m_contentWidget)
    {
        return hint;
    }

    const int frame = 2 * frameWidth();
    int width = m_contentWidget->sizeHint().width() + frame;
    int height = m_contentWidget->sizeHint().height() + frame;

    if (verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
    {
        width += verticalScrollBar()->sizeHint().width();
    }
    if (horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
    {
        height += horizontalScrollBar()->sizeHint().height();
    }

    hint.setWidth(qMax(hint.width(), width));
    hint.setHeight(m_maximumContentHeight > 0 ? qMin(height, m_maximumContentHeight) : height);
    return hint;
}

QSize AdjustingScrollArea::minimumSizeHint() const
{
    QSize hint = QScrollArea::minimumSizeHint();
    if (!m_contentWidget)
    {
        return hint;
    }

    int width = m_contentWidget->minimumSizeHint().width() + 2 * frameWidth();
    if (verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)
    {
        width += verticalScrollBar()->sizeHint().width();
    }
    hint.setWidth(qMax(hint.width(), width));
    if (m_maximumContentHeight > 0)
    {
        hint.setHeight(qMin(hint.height(), m_maximumContentHeight));
    }
    return hint;
}

bool AdjustingScrollArea::eventFilter(QObject* watched, QEvent* event)
{
    if (isContentGeometryChange(watched, event))
    {
        scheduleGeometryUpdate();
    }
    return QScrollArea::eventFilter(watched, event);
}

bool AdjustingScrollArea::isContentGeometryChange(QObject* watched, QEvent* event) const
{
    if (watched == m_contentWidget)
    {
        return event->type() == QEvent::LayoutRequest ||
               event->type() == QEvent::Resize;
    }
    if (watched == viewport())
    {
        return event->type() == QEvent::Resize;
    }
    return false;
}

void AdjustingScrollArea::scheduleGeometryUpdate()
{
    if (m_geometryUpdatePending)
    {
        return;
    }

    m_geometryUpdatePending = true;
    // 在当前事件循环轮转结束后触发,让Qt把同一拍内
    // 累积的多个 LayoutRequest / Resize 全部消化完，回调运行时 sizeHint
    // 已是最终态，读到的尺寸值稳定。回调里先清标志再 updateGeometry，是为了让
    // 工作过程中若再引发 LayoutRequest 仍可正常预约下一轮。
    QTimer::singleShot(0, this, [this] {
        m_geometryUpdatePending = false;
        updateGeometry();
    });
}
