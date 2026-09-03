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
#ifndef __PRINTER_SUBITEM_H__
#define __PRINTER_SUBITEM_H__

#include <QObject>

#include "panel-interface.h"
#include "plugin-subitem-interface.h"
#include "printer-manager.h"
#include "printer-page.h"

class PrinterSubItem : public QObject,
                       public KiranControlPanel::PluginSubitemInterface
{
    Q_OBJECT
public:
    explicit PrinterSubItem(QObject *parent = nullptr) : QObject(parent)
    {
    }
    ~PrinterSubItem()
    {
    }

public:
    QString getID() override
    {
        return "Printer";
    }

    QString getName() override
    {
        return tr("Printer Settings");
    }

    QString getCategory() override
    {
        return "hardware";
    }

    QString getDesc() override
    {
        return "";
    }

    QString getIcon() override
    {
        return "ksvg-kcp-printer";
    }

    int getWeight() override
    {
        return 0;
    }

    QWidget *createWidget() override
    {
        return new PrinterPage(new PrinterManager());
    }

    QVector<QPair<QString, QString>> getSearchKeys() override
    {
        return {};
    }

    bool jumpToSearchEntry(const QString &key) override
    {
        Q_UNUSED(key);
        return false;
    }

    bool haveUnSavedOptions() override
    {
        return false;
    }
};

#endif  // __PRINTER_SUBITEM_H__
