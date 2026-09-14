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
#pragma once

#include "font-info.h"

#include <QAbstractTableModel>
#include <QEvent>
#include <QMap>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QTableView>

enum FontTableField
{
    FONT_TABLE_FIELD_CHECKBOX,
    FONT_TABLE_FIELD_NAME,
    FONT_TABLE_FIELD_STYLE,
    FONT_TABLE_FIELD_SOURCE,
    FONT_TABLE_FIELD_STATUS,
    FONT_TABLE_FIELD_LAST
};

enum FontTableRole
{
    FontEnabledRole = Qt::UserRole + 1
};

class CustomHeaderView;

class FontDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FontDelegate(QObject *parent = nullptr);
    ~FontDelegate() override = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    QMap<QModelIndex, QRect> m_checkboxRects;
};

class FontFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FontFilterModel(QObject *parent = nullptr);
    ~FontFilterModel() override = default;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

class FontModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FontModel(QObject *parent = nullptr);
    ~FontModel() override = default;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QList<FontInfo> fonts() const;
    FontInfo fontAt(int row) const;
    void updateRecord(const QList<FontInfo> &fonts);

private:
    QList<FontInfo> m_fonts;
};

class FontTable : public QTableView
{
    Q_OBJECT
public:
    explicit FontTable(QWidget *parent = nullptr);
    ~FontTable() override = default;

    void searchTextChanged(const QString &text);
    void setFonts(const QList<FontInfo> &fonts);
    QList<FontInfo> selectedFonts() const;
    FontInfo fontAt(const QPoint &pos) const;
    void clearTable();

signals:
    void fontSelectedChanged(int selectedCount);

protected:
    bool viewportEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void updateColumnWidths();
    void checkedAllItem(Qt::CheckState checkState);
    void updateHeaderState();
    void setHoverRow(int row);

private:
    FontFilterModel *m_filterProxy;
    FontModel *m_model;
    FontDelegate *m_delegate;
    CustomHeaderView *m_headerView;
    int m_hoverRow = -1;
};
