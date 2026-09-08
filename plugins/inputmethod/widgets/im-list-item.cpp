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

#include "im-list-item.h"

#include <style-palette.h>

#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

using namespace Kiran;

namespace
{
// 条目几何常量
const int kItemHeight = 40;
const int kMarginLeft = 12;
const int kMarginRight = 12;
const int kIconSize = 32;
const int kIconTextGap = 12;
const int kDeleteButtonSize = 16;
const int kSpacing = 8;
}  // namespace

ImListItem::ImListItem(const ImEntry& entry, QWidget* parent)
    : QWidget(parent),
      m_entry(entry)
{
    initUI();
}

ImListItem::~ImListItem()
{
}

void ImListItem::initUI()
{
    setObjectName("ImListItem");
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    setFixedHeight(kItemHeight);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setCursor(Qt::ArrowCursor);
}

void ImListItem::setEntry(const ImEntry& entry)
{
    m_entry = entry;
    update();
}

void ImListItem::setDeletable(bool deletable)
{
    if (m_deletable == deletable)
    {
        return;
    }
    m_deletable = deletable;
    if (!m_deletable)
    {
        m_deleteHovered = false;
        m_pressedOnDelete = false;
        setToolTip(QString());
    }
    update();
}

QString ImListItem::primaryText() const
{
    if (!m_entry.name.isEmpty())
    {
        return m_entry.name;
    }
    if (!m_entry.nativeName.isEmpty())
    {
        return m_entry.nativeName;
    }
    return m_entry.uniqueName;
}

QSize ImListItem::sizeHint() const
{
    return QSize(320, kItemHeight);
}

QSize ImListItem::minimumSizeHint() const
{
    return QSize(160, kItemHeight);
}

QRect ImListItem::deleteButtonRect() const
{
    int x = width() - kMarginRight - kDeleteButtonSize;
    int y = (height() - kDeleteButtonSize) / 2;
    return QRect(x, y, kDeleteButtonSize, kDeleteButtonSize);
}

void ImListItem::enterEvent(QEvent* event)
{
    m_hovered = true;
    update();
    QWidget::enterEvent(event);
}

void ImListItem::leaveEvent(QEvent* event)
{
    m_hovered = false;
    m_deleteHovered = false;
    m_pressed = false;
    m_pressedOnDelete = false;
    setCursor(Qt::ArrowCursor);
    setToolTip(QString());
    update();
    QWidget::leaveEvent(event);
}

void ImListItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_deletable && m_hovered && deleteButtonRect().contains(event->pos()))
        {
            m_pressed = false;
            m_pressedOnDelete = true;
            event->accept();
            return;
        }

        m_pressed = true;
        m_pressedOnDelete = false;
        m_pressPos = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void ImListItem::mouseMoveEvent(QMouseEvent* event)
{
    bool deleteHovered = m_deletable && m_hovered && deleteButtonRect().contains(event->pos());
    if (m_deleteHovered != deleteHovered)
    {
        m_deleteHovered = deleteHovered;
        setCursor(m_deleteHovered ? Qt::PointingHandCursor : Qt::ArrowCursor);
        setToolTip(m_deleteHovered ? tr("Remove") : QString());
        update();
    }

    if (m_pressed && (event->buttons() & Qt::LeftButton))
    {
        int distance = (event->pos() - m_pressPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
        {
            m_pressed = false;
            emit dragStarted(this);
        }
    }
    QWidget::mouseMoveEvent(event);
}

void ImListItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_pressedOnDelete)
    {
        m_pressedOnDelete = false;
        m_pressed = false;
        if (m_deletable && deleteButtonRect().contains(event->pos()))
        {
            emit deleteClicked(m_entry);
            event->accept();
            return;
        }
    }

    m_pressed = false;
    m_pressedOnDelete = false;
    QWidget::mouseReleaseEvent(event);
}

void ImListItem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    StylePalette* palette = StylePalette::instance();
    StylePalette::ColorState group = isEnabled() ? StylePalette::Active : StylePalette::Disabled;

    // 背景：悬浮时使用控件悬浮背景色
    QRectF backgroundRect = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(backgroundRect, 6, 6);

    QColor widgetColor = palette->color(m_hovered ? StylePalette::Hover : group,
                                        StylePalette::Widget, StylePalette::Background);
    painter.fillPath(backgroundPath, widgetColor);

    QColor borderColor = palette->color(group, StylePalette::Widget, StylePalette::Border);
    painter.setPen(borderColor);
    painter.drawPath(backgroundPath);

    // 图标
    QRect iconRect(kMarginLeft, (height() - kIconSize) / 2, kIconSize, kIconSize);
    QIcon icon = QIcon::fromTheme(m_entry.iconName);
    if (icon.isNull())
    {
        icon = QIcon::fromTheme(m_entry.isKeyboard ? "input-keyboard" : "ksvg-kcp-input-method");
    }
    if (!icon.isNull())
    {
        icon.paint(&painter, iconRect);
    }
    else if (!m_entry.label.isEmpty())
    {
        // 无可用图标时退化为绘制托盘短标签
        QColor iconColor = palette->color(group, StylePalette::Widget, StylePalette::Foreground);
        painter.setPen(iconColor);
        painter.drawText(iconRect, Qt::AlignCenter, m_entry.label);
    }

    // 文本区域范围：右侧需要给删除按钮留位
    int textRight = width() - kMarginRight - kDeleteButtonSize - kSpacing;
    int textLeft = iconRect.right() + 1 + kIconTextGap;
    int textWidth = qMax(0, textRight - textLeft);

    QColor textColor = palette->color(group, StylePalette::Widget, StylePalette::Foreground);

    QFont primaryFont = font();
    painter.setFont(primaryFont);
    painter.setPen(textColor);

    QRect textRect(textLeft, 0, textWidth, height());
    QFontMetrics fm(primaryFont);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                     fm.elidedText(primaryText(), Qt::ElideRight, textWidth));

    // 删除按钮：仅条目悬浮且可删除时绘制，避免引入默认按钮背景样式。
    // 图标用控制面板主程序内嵌的统一资源（与键盘布局/快捷键等页删除按钮一致），
    // 不用主题 ksvg-trash：该图标仅随 kiran-control-panel 2.8 包安装，2.6 环境
    // 缺失会导致 QIcon::fromTheme 返回空、删除按钮不渲染，移除入口不可见。
    if (m_deletable && m_hovered)
    {
        QString deleteIconPath = ":/kiran-control-panel/images/trash.svg";
        if (m_pressedOnDelete)
        {
            deleteIconPath = ":/kiran-control-panel/images/trash-pressed.svg";
        }
        else if (m_deleteHovered)
        {
            deleteIconPath = ":/kiran-control-panel/images/trash-hover.svg";
        }
        QIcon deleteIcon(deleteIconPath);
        if (!deleteIcon.isNull())
        {
            deleteIcon.paint(&painter, deleteButtonRect(), Qt::AlignCenter);
        }
    }
}
