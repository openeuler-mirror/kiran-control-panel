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

#include "cups-wrapper.h"
#include "printer-info.h"

class QTimer;

// CUPS / systemd D-Bus / scp 等耗时操作的后台执行者。
// 该对象被 moveToThread 到 PrinterManager 的专用工作线程：所有 CUPS IPP、
// systemd D-Bus、scp D-Bus 调用都只在本对象所在线程执行，避免阻塞主线程 UI。
class PrinterWorker : public QObject
{
    Q_OBJECT

public:
    explicit PrinterWorker(QObject *parent = nullptr);
    ~PrinterWorker() override;

public slots:
    // 启动：建立服务监听、订阅、首刷并启动周期轮询（本线程内串行执行）
    void start();
    void stop();
    // 停止并释放线程内对象（仅本对象线程内调用；供所有者析构收尾）
    void cleanup();
    // 手动刷新一轮（添加向导结束、按钮点击后调用）
    void poll();

    void setDefaultPrinter(const QString &printerName);
    void printTestPage(const QString &printerName, const QString &title);
    void startService();
    void openProperties(quint32 xid, const QString &printerName);
    void openPrintQueue(const QString &printerName);
    void openManagePrinter();

signals:
    void serviceAvailableChanged(bool available);
    void printersChanged(const QList<PrinterInfo> &printers);

private:
    void subscribe();
    void unsubscribe();
    void refreshPrinters();
    void onPollTimer();
    void handleDrain(const CupsWrapper::NotificationDrain &drain);
    void handleServiceAvailableChanged(bool available);
    void applyAvailability(bool available);

    CupsWrapper *m_cups = nullptr;  // 在本对象线程创建，随本对象析构
    QTimer *m_timer = nullptr;      // 周期轮询（本对象线程内触发）
    bool m_running = false;         // 生命周期开关：start 后才允许请求与轮询
    bool m_available = false;       // CUPS 服务可用快照（systemd 信号 / 手动对账维护）
    bool m_hasSnapshot = false;     // 是否已上报过快照（首刷无条件上报一次）
    QList<PrinterInfo> m_lastPrinters;  // 上次上报的列表快照，用于变化比对
};
