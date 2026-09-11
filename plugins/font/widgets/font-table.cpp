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
#include "font-table.h"
#include "custom-header-view.h"

#include <style-palette.h>

#include <QApplication>
#include <QHeaderView>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleOptionButton>

FontDelegate::FontDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void FontDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto widget = option.widget;
    auto style = widget ? widget->style() : QApplication::style();

    if (index.column() == FONT_TABLE_FIELD_CHECKBOX)
    {
        bool checked = index.data(Qt::EditRole).toBool();
        QStyleOptionButton checkboxOption;

        int indicatorSize = style->pixelMetric(QStyle::PM_IndicatorWidth, &option, widget);
        QRect checkboxRect = QRect(0, 0, indicatorSize, indicatorSize);
        checkboxRect.moveCenter(option.rect.center());
        const_cast<FontDelegate *>(this)->m_checkboxRects[index] = checkboxRect;
        checkboxOption.rect = checkboxRect;
        checkboxOption.state = QStyle::State_Enabled;

        if (checked)
            checkboxOption.state |= QStyle::State_On;
        else
            checkboxOption.state |= QStyle::State_Off;

        if (option.state & QStyle::State_MouseOver)
            checkboxOption.state |= QStyle::State_MouseOver;
        if (option.state & QStyle::State_Selected)
            checkboxOption.state |= QStyle::State_Selected;

        style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkboxOption, painter, widget);
        return;
    }

    if (index.column() == FONT_TABLE_FIELD_STATUS)
    {
        QStyleOptionViewItem viewOption(option);

        const bool enabled = index.data(FontEnabledRole).toBool();
        const QString text = index.data(Qt::DisplayRole).toString();
        const int dotSize = 8;
        const int leftPadding = 8;
        const int spacing = 6;

        QRect dotRect(option.rect.left() + leftPadding,
                      option.rect.center().y() - dotSize / 2,
                      dotSize, dotSize);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(enabled ? QColor("#3DCC6F") : QColor("#9A9A9A"));
        painter->drawEllipse(dotRect);
        painter->restore();

        QRect textRect = option.rect.adjusted(leftPadding + dotSize + spacing, 0, -4, 0);
        style->drawItemText(painter, textRect, Qt::AlignLeft | Qt::AlignVCenter,
                            viewOption.palette, viewOption.state & QStyle::State_Enabled, text);

        return;
    }

    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption, index);
    viewOption.features &= ~QStyleOptionViewItem::WrapText;
    QStyledItemDelegate::paint(painter, viewOption, index);
}

bool FontDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    auto mouseEvent = static_cast<QMouseEvent *>(event);

    if (event->type() == QEvent::MouseButtonPress &&
        option.rect.contains(mouseEvent->pos()) &&
        index.column() == FONT_TABLE_FIELD_CHECKBOX &&
        m_checkboxRects.contains(index))
    {
        auto value = model->data(index, Qt::EditRole).toBool();
        model->setData(index, !value, Qt::EditRole);
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

FontFilterModel::FontFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool FontFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const QString keyword = filterRegExp().pattern();
    if (keyword.isEmpty())
        return true;

    auto matchColumn = [this, sourceRow, &sourceParent, &keyword](int column)
    {
        const QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
        return sourceModel()->data(index, Qt::DisplayRole).toString().contains(keyword, Qt::CaseInsensitive);
    };

    return matchColumn(FONT_TABLE_FIELD_NAME) ||
           matchColumn(FONT_TABLE_FIELD_STYLE) ||
           matchColumn(FONT_TABLE_FIELD_SOURCE) ||
           matchColumn(FONT_TABLE_FIELD_STATUS);
}

FontModel::FontModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int FontModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_fonts.size();
}

int FontModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return FONT_TABLE_FIELD_LAST;
}

QVariant FontModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    const int column = index.column();
    if (row >= m_fonts.size() || column >= FONT_TABLE_FIELD_LAST)
        return QVariant();

    const FontInfo &info = m_fonts.at(row);

    switch (role)
    {
    case Qt::DisplayRole:
        switch (column)
        {
        case FONT_TABLE_FIELD_NAME:
            return info.family;
        case FONT_TABLE_FIELD_STYLE:
            return info.style;
        case FONT_TABLE_FIELD_SOURCE:
            return info.isSystem ? tr("System") : tr("User");
        case FONT_TABLE_FIELD_STATUS:
            return info.enabled ? tr("Enabled") : tr("Disabled");
        default:
            break;
        }
        break;
    case Qt::EditRole:
        if (column == FONT_TABLE_FIELD_CHECKBOX)
            return info.selected;
        break;
    case Qt::ToolTipRole:
        switch (column)
        {
        case FONT_TABLE_FIELD_NAME:
        case FONT_TABLE_FIELD_STYLE:
        case FONT_TABLE_FIELD_SOURCE:
            return data(index, Qt::DisplayRole);
        default:
            break;
        }
        break;
    case FontEnabledRole:
        return info.enabled;
    case Qt::TextAlignmentRole:
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    default:
        break;
    }

    return QVariant();
}

QVariant FontModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
    case FONT_TABLE_FIELD_NAME:
        return tr("Font Name");
    case FONT_TABLE_FIELD_STYLE:
        return tr("Font Style");
    case FONT_TABLE_FIELD_SOURCE:
        return tr("Source");
    case FONT_TABLE_FIELD_STATUS:
        return tr("Status");
    default:
        break;
    }
    return QVariant();
}

bool FontModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    if (!index.isValid() || index.column() != FONT_TABLE_FIELD_CHECKBOX)
        return false;

    m_fonts[index.row()].selected = value.toBool();
    emit dataChanged(index, index);
    return true;
}

QList<FontInfo> FontModel::fonts() const
{
    return m_fonts;
}

FontInfo FontModel::fontAt(int row) const
{
    if (row < 0 || row >= m_fonts.size())
        return FontInfo();
    return m_fonts.at(row);
}

void FontModel::updateRecord(const QList<FontInfo> &fonts)
{
    beginResetModel();
    m_fonts = fonts;
    endResetModel();
}

FontTable::FontTable(QWidget *parent)
    : QTableView(parent)
{
    viewport()->setAutoFillBackground(false);
    viewport()->setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    m_model = new FontModel(this);

    m_headerView = new CustomHeaderView(this);
    setHorizontalHeader(m_headerView);

    m_filterProxy = new FontFilterModel(this);
    m_filterProxy->setSourceModel(m_model);
    setModel(m_filterProxy);

    m_delegate = new FontDelegate(this);
    setItemDelegate(m_delegate);

    connect(m_headerView, &CustomHeaderView::toggled, this, &FontTable::checkedAllItem);
    connect(m_model, &FontModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight)
            {
                if (topLeft.column() > FONT_TABLE_FIELD_CHECKBOX ||
                    bottomRight.column() < FONT_TABLE_FIELD_CHECKBOX)
                    return;
                updateHeaderState();
                emit fontSelectedChanged(selectedFonts().size());
            });
    connect(m_filterProxy, &FontFilterModel::modelReset, this, [this]()
            {
                m_headerView->setCheckState(Qt::Unchecked);
                emit fontSelectedChanged(selectedFonts().size());
            });

    m_headerView->setSectionResizeMode(FONT_TABLE_FIELD_CHECKBOX, QHeaderView::Fixed);
    m_headerView->setSectionResizeMode(FONT_TABLE_FIELD_NAME, QHeaderView::Interactive);
    m_headerView->setSectionResizeMode(FONT_TABLE_FIELD_STYLE, QHeaderView::Interactive);
    m_headerView->setSectionResizeMode(FONT_TABLE_FIELD_SOURCE, QHeaderView::Interactive);
    m_headerView->setSectionResizeMode(FONT_TABLE_FIELD_STATUS, QHeaderView::Interactive);
    m_headerView->setStretchLastSection(true);
    m_headerView->setSectionsMovable(false);
    m_headerView->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_headerView->setHighlightSections(false);
    m_headerView->setFixedHeight(40);

    setSelectionMode(QAbstractItemView::NoSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFocusPolicy(Qt::NoFocus);
    setShowGrid(false);

    verticalHeader()->setVisible(false);
    verticalHeader()->setDefaultSectionSize(60);
}

void FontTable::searchTextChanged(const QString &text)
{
    m_filterProxy->setFilterFixedString(text);
    updateHeaderState();
    emit fontSelectedChanged(selectedFonts().size());
}

void FontTable::setFonts(const QList<FontInfo> &fonts)
{
    m_model->updateRecord(fonts);
}

QList<FontInfo> FontTable::selectedFonts() const
{
    QList<FontInfo> selected;
    for (int i = 0; i < m_filterProxy->rowCount(); ++i)
    {
        const QModelIndex proxyIndex = m_filterProxy->index(i, FONT_TABLE_FIELD_CHECKBOX);
        if (!m_filterProxy->data(proxyIndex, Qt::EditRole).toBool())
            continue;
        const QModelIndex sourceIndex = m_filterProxy->mapToSource(proxyIndex);
        selected.append(m_model->fontAt(sourceIndex.row()));
    }
    return selected;
}

FontInfo FontTable::fontAt(const QPoint &pos) const
{
    const QModelIndex proxyIndex = indexAt(pos);
    if (!proxyIndex.isValid())
        return FontInfo();
    return m_model->fontAt(m_filterProxy->mapToSource(proxyIndex).row());
}

void FontTable::clearTable()
{
    m_model->updateRecord(QList<FontInfo>());
}

void FontTable::setHoverRow(int row)
{
    if (m_hoverRow == row)
        return;
    m_hoverRow = row;
    viewport()->update();
}

bool FontTable::viewportEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseMove:
    {
        const auto *mouseEvent = static_cast<QMouseEvent *>(event);
        const QModelIndex index = indexAt(mouseEvent->pos());
        setHoverRow(index.isValid() ? index.row() : -1);
        break;
    }
    case QEvent::Leave:
        setHoverRow(-1);
        break;
    default:
        break;
    }
    return QTableView::viewportEvent(event);
}

void FontTable::paintEvent(QPaintEvent *event)
{
    if (m_hoverRow >= 0 && m_hoverRow < m_filterProxy->rowCount())
    {
        QRect rowRect;
        for (int col = 0; col < m_filterProxy->columnCount(); ++col)
            rowRect = rowRect.united(visualRect(m_filterProxy->index(m_hoverRow, col)));

        if (rowRect.isValid())
        {
            const QColor hoverColor = Kiran::StylePalette::instance()->color(Kiran::StylePalette::Hover,
                                                                             Kiran::StylePalette::Widget,
                                                                             Kiran::StylePalette::Background);
            QPainter painter(viewport());
            painter.fillRect(rowRect, hoverColor);
        }
    }

    QTableView::paintEvent(event);
}

void FontTable::checkedAllItem(Qt::CheckState checkState)
{
    for (int i = 0; i < m_filterProxy->rowCount(); ++i)
    {
        const QModelIndex proxyIndex = m_filterProxy->index(i, FONT_TABLE_FIELD_CHECKBOX);
        const QModelIndex sourceIndex = m_filterProxy->mapToSource(proxyIndex);
        m_model->setData(sourceIndex, checkState == Qt::Checked, Qt::EditRole);
    }
}

void FontTable::updateHeaderState()
{
    int checkedCount = 0;
    const int totalCount = m_filterProxy->rowCount();
    for (int i = 0; i < totalCount; ++i)
    {
        const QModelIndex proxyIndex = m_filterProxy->index(i, FONT_TABLE_FIELD_CHECKBOX);
        if (m_filterProxy->data(proxyIndex, Qt::EditRole).toBool())
            ++checkedCount;
    }

    Qt::CheckState state = Qt::Unchecked;
    if (totalCount > 0 && checkedCount == totalCount)
        state = Qt::Checked;
    else if (checkedCount > 0)
        state = Qt::PartiallyChecked;

    m_headerView->setCheckState(state);
}

void FontTable::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateColumnWidths();
}

void FontTable::showEvent(QShowEvent *event)
{
    QTableView::showEvent(event);
    updateColumnWidths();
}

void FontTable::updateColumnWidths()
{
    int availableWidth = viewport()->width();
    if (verticalScrollBar()->isVisible())
        availableWidth -= verticalScrollBar()->width();

    const int checkboxWidth = 50;
    int remainingWidth = availableWidth - checkboxWidth;
    if (remainingWidth <= 0)
        return;

    const int nameWeight = 120;
    const int styleWeight = 80;
    const int sourceWeight = 60;
    const int statusWeight = 80;
    const int totalWeight = nameWeight + styleWeight + sourceWeight + statusWeight;

    const int nameWidth = (remainingWidth * nameWeight) / totalWeight;
    const int styleWidth = (remainingWidth * styleWeight) / totalWeight;
    const int sourceWidth = (remainingWidth * sourceWeight) / totalWeight;
    const int statusWidth = remainingWidth - nameWidth - styleWidth - sourceWidth;

    m_headerView->resizeSection(FONT_TABLE_FIELD_CHECKBOX, checkboxWidth);
    m_headerView->resizeSection(FONT_TABLE_FIELD_NAME, nameWidth);
    m_headerView->resizeSection(FONT_TABLE_FIELD_STYLE, styleWidth);
    m_headerView->resizeSection(FONT_TABLE_FIELD_SOURCE, sourceWidth);
    m_headerView->resizeSection(FONT_TABLE_FIELD_STATUS, statusWidth);
}
