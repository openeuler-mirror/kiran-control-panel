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

#include "printer-info.h"

#include <kiran-frame/kiran-frame.h>

class QLabel;
class QPushButton;
class QToolButton;
class QWidget;

class PrinterCard : public KiranFrame
{
    Q_OBJECT

public:
    explicit PrinterCard(const PrinterInfo &info, QWidget *parent = nullptr);

    const PrinterInfo &info() const { return m_info; }
    void setInfo(const PrinterInfo &info);

signals:
    void setDefaultClicked(const QString &printerName);
    void printTestPageClicked(const QString &printerName);
    void propertiesClicked(const QString &printerName);
    void viewQueueClicked(const QString &printerName);
    void managePrinterClicked();

private:
    void initUi();
    void refreshUi();
    void applyTheme();

private:
    PrinterInfo m_info;
    QLabel *m_iconLabel = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_stateDot = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_modelLabel = nullptr;
    QWidget *m_defaultBox = nullptr;
    QLabel *m_starLabel = nullptr;
    QLabel *m_defaultBadge = nullptr;
    QPushButton *m_setDefaultButton = nullptr;
};
