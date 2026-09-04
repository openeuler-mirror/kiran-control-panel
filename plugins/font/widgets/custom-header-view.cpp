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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */
#include "custom-header-view.h"
#include "font-table.h"

#include <palette.h>

#include <QAbstractItemModel>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

using namespace Kiran::Theme;

CustomHeaderView::CustomHeaderView(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent),
      m_checkState(Qt::Unchecked)
{
    setSectionsClickable(true);
    setMouseTracking(true);
}

CustomHeaderView::~CustomHeaderView()
{
}

void CustomHeaderView::setCheckState(Qt::CheckState checkState)
{
    if (m_checkState != checkState)
    {
        m_checkState = checkState;
        viewport()->update();
    }
}

void CustomHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();

    auto kiranPalette = DEFAULT_PALETTE();
    QColor backgroundColor = kiranPalette->getColor(Palette::ColorGroup::ACTIVE, Palette::ColorRole::WIDGET);
    QColor textColor = kiranPalette->getColor(Palette::ColorGroup::ACTIVE, Palette::ColorRole::TEXT);

    painter->fillRect(rect, backgroundColor);

    if (logicalIndex == FONT_TABLE_FIELD_CHECKBOX)
    {
        auto widget = const_cast<CustomHeaderView *>(this);
        auto style = widget->style();

        QStyleOptionButton checkboxOption;
        int indicatorSize = style->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, widget);
        QRect checkboxRect = QRect(0, 0, indicatorSize, indicatorSize);
        checkboxRect.moveCenter(rect.center());
        const_cast<CustomHeaderView *>(this)->m_checkboxRect = checkboxRect;

        checkboxOption.rect = checkboxRect;
        checkboxOption.state = QStyle::State_Enabled;
        if (m_checkState == Qt::Checked)
            checkboxOption.state |= QStyle::State_On;
        else if (m_checkState == Qt::PartiallyChecked)
            checkboxOption.state |= QStyle::State_NoChange;
        else
            checkboxOption.state |= QStyle::State_Off;

        style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkboxOption, painter, widget);
    }
    else
    {
        QString text = model() ? model()->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString() : QString();
        painter->setPen(textColor);
        QRect textRect = rect.adjusted(10, 0, 0, 0);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    }

    painter->restore();
}

void CustomHeaderView::mousePressEvent(QMouseEvent *e)
{
    int column = logicalIndexAt(e->pos());
    if (column == FONT_TABLE_FIELD_CHECKBOX && m_checkboxRect.contains(e->pos()))
    {
        m_checkState = (m_checkState == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
        emit toggled(m_checkState);
        viewport()->update();
        return;
    }

    QHeaderView::mousePressEvent(e);
}

void CustomHeaderView::mouseMoveEvent(QMouseEvent *e)
{
    int column = logicalIndexAt(e->pos());
    if (column == FONT_TABLE_FIELD_CHECKBOX && m_checkboxRect.contains(e->pos()))
        setCursor(Qt::PointingHandCursor);
    else
        setCursor(Qt::ArrowCursor);

    QHeaderView::mouseMoveEvent(e);
}
