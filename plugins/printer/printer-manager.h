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

#include <QObject>

#include "printer-info.h"

class QProcess;
class QThread;
class PrinterWorker;

// 打印机管理的前端编排者（运行在主线程/UI 线程）：
// 所有 CUPS / systemd D-Bus / scp 等耗时操作交由后台 PrinterWorker 异步执行，
// 结果经队列信号回主线程更新 UI，不阻塞界面。
class PrinterManager : public QObject
{
    Q_OBJECT

public:
    explicit PrinterManager(QObject *parent = nullptr);
    ~PrinterManager() override;

    void start();
    void stop();
    void poll();

    void setDefault(const QString &printerName);
    void printTestPage(const QString &printerName, const QString &title);
    void startService();
    bool openAddDialog(quint32 xid);
    void openProperties(quint32 xid, const QString &printerName);
    void openPrintQueue(const QString &printerName);
    void openManagePrinter();

signals:
    void serviceStatusChanged(bool available);
    void printersChanged(const QList<PrinterInfo> &printers);

private:
    QThread *m_workerThread = nullptr;
    PrinterWorker *m_worker = nullptr;   // 后台执行者，生存期由 m_workerThread 管理
    QProcess *m_addProcess = nullptr;    // 添加向导进程（主线程跟踪）
};
