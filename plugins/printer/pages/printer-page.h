/**
 * Copyright (c) 2026 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     gaobo <gaobo@kylinsec.com.cn>
 */
#pragma once

#include <QList>
#include <QWidget>

#include "printer-info.h"

namespace Ui
{
class PrinterPage;
}

class PrinterCard;
class PrinterManager;

class PrinterPage : public QWidget
{
    Q_OBJECT

public:
    explicit PrinterPage(PrinterManager *manager, QWidget *parent = nullptr);
    ~PrinterPage();

    QSize sizeHint() const override;

private slots:
    void handleRefresh();
    void handleAddPrinterClicked();
    void handleStartServiceClicked();
    void handleSearchTextChanged(const QString &text);

    void handleServiceStatusChanged(bool available);
    void handlePrintersChanged(const QList<PrinterInfo> &printers);

    void handleSetDefaultClicked(const QString &printerName);
    void handlePrintTestPageClicked(const QString &printerName);
    void handlePropertiesClicked(const QString &printerName);
    void handleViewQueueClicked(const QString &printerName);

private:
    void initUiStyle();
    void applyTheme();
    void initConnect();
    void showNormalState(const QList<PrinterInfo> &printers);
    void showErrorState();
    void setServiceControlsEnabled(bool enabled);
    void clearPrinterCards();
    void rebuildPrinterCards(const QList<PrinterInfo> &printers);
    void syncCards(const QList<PrinterInfo> &printers);
    void applySearchFilter();
    bool printerMatchesKeyword(const PrinterInfo &info, const QString &keyword) const;
    void bindCard(PrinterCard *card);

private:
    Ui::PrinterPage *ui;
    PrinterManager *m_manager;
    QList<PrinterInfo> m_allPrinters;
    QList<PrinterCard *> m_cards;
    bool m_serviceAvailable = true;
};
