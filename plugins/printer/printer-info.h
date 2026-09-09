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

#include <QMetaType>
#include <QString>

struct PrinterInfo
{
    QString name;
    QString model;  // 优先 printer-info；raw 队列 make-and-model 常含 Local，会干扰搜索
    bool isDefault = false;
    bool isOnline = false;

    bool operator==(const PrinterInfo &other) const
    {
        return name == other.name && model == other.model && isDefault == other.isDefault &&
               isOnline == other.isOnline;
    }
    bool operator!=(const PrinterInfo &other) const { return !(*this == other); }
};

// 供跨线程 queued 信号传递（PrinterWorker → PrinterManager），须在运行时 qRegisterMetaType
Q_DECLARE_METATYPE(PrinterInfo)
