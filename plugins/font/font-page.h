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
#ifndef FONT_PAGE_H
#define FONT_PAGE_H

#include "font-info.h"

#include <QWidget>

namespace Ui
{
class FontPage;
}

class FontPage : public QWidget
{
    Q_OBJECT
public:
    explicit FontPage(QWidget *parent = nullptr);
    ~FontPage();

    QSize sizeHint() const override;
    static QWidget *createPage();

private:
    void initUI();
    void initConnections();
    void reloadFonts();
    void updateDeleteButtonState();
    QList<FontInfo> targetFonts(const QPoint &contextPos = QPoint()) const;
    QStringList selectFontFiles();
    void addFonts(bool system);
    void deleteFonts(const QList<FontInfo> &fonts);
    void setFontsEnabled(const QList<FontInfo> &fonts, bool enabled);
    void openFontViewer(const FontInfo &info);
    void showError(const QString &message);

private slots:
    void onAddSystemFont();
    void onAddUserFont();
    void onDeleteFonts();
    void onTableContextMenu(const QPoint &pos);

private:
    Ui::FontPage *ui;
};

#endif  // FONT_PAGE_H
