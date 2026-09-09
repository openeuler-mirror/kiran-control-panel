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

#include <QString>

class QProcess;

class ScpWrapper
{
public:
    static bool openProperties(quint32 xid, const QString &printerName);
    static bool openPrintQueue(const QString &printerName);
    static bool openManagePrinter();
    static bool openAddPrinter();

    // 以受控方式启动添加向导，返回进程对象（调用方负责 setParent/连接 finished），
    // 依赖缺失或启动失败返回 nullptr。用于在向导结束时立即刷新列表。
    static QProcess *startAddPrinterTracked();
};
