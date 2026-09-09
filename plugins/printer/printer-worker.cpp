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
#include "printer-worker.h"

#include "cups-wrapper.h"
#include "scp-wrapper.h"

#include <QTimer>

#include <QtGlobal>

namespace
{
constexpr int kDefaultNotifyDrainIntervalMs = 1000;
}

PrinterWorker::PrinterWorker(QObject *parent)
    : QObject(parent)
{
}

PrinterWorker::~PrinterWorker()
{
    // CupsWrapper/QTimer 均在本对象所在线程内创建并随本对象清理
    delete m_timer;
    delete m_cups;
}

void PrinterWorker::start()
{
    // 已启动则忽略重复 start（页面可能多次进入）
    if (m_running)
    {
        return;
    }
    m_running = true;

    if (!m_cups)
    {
        // 在本线程创建 CUPS 包装与轮询定时器，确保所有网络调用只在本线程执行
        m_cups = new CupsWrapper();
        connect(m_cups, &CupsWrapper::serviceAvailableChanged,
                this, &PrinterWorker::handleServiceAvailableChanged);

        m_timer = new QTimer();
        m_timer->setInterval(kDefaultNotifyDrainIntervalMs);
        connect(m_timer, &QTimer::timeout, this, &PrinterWorker::onPollTimer);
    }

    m_cups->startServiceWatch();
    // startServiceWatch 内已同步初读一次状态并可能发信号；这里按当前快照应用一次
    applyAvailability(m_cups->serviceAvailable());
}

void PrinterWorker::stop()
{
    if (!m_running)
    {
        return;
    }
    m_running = false;
    if (m_timer)
    {
        m_timer->stop();
    }
    if (m_cups)
    {
        unsubscribe();
        m_cups->stopServiceWatch();
    }
}

void PrinterWorker::cleanup()
{
    if (m_timer)
    {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
    if (m_cups)
    {
        // 退出前取消订阅（避免服务端订阅残留到租约到期），再停止服务监听
        unsubscribe();
        m_cups->stopServiceWatch();
        delete m_cups;
        m_cups = nullptr;
    }
    m_running = false;
}

void PrinterWorker::poll()
{
    if (!m_cups)
    {
        return;
    }
    applyAvailability(m_cups->serviceAvailable());
    // 服务不可用时仅对账状态（applyAvailability 内处理）；可用才确保订阅并取列表
    if (m_available)
    {
        subscribe();
        refreshPrinters();
    }
}

void PrinterWorker::setDefaultPrinter(const QString &printerName)
{
    if (m_cups && m_available)
    {
        if (m_cups->setDefaultPrinter(printerName))
        {
            // 设默认成功：立即重取列表，让 UI 上的默认标记及时更新
            refreshPrinters();
        }
    }
}

void PrinterWorker::printTestPage(const QString &printerName, const QString &title)
{
    if (m_cups && m_available)
    {
        m_cups->printTestPage(printerName, title);
    }
}

void PrinterWorker::startService()
{
    if (m_cups && m_cups->startService())
    {
        // 启动成功后等待 systemd ActiveState 变化信号；这里主动刷新一次可用性
        applyAvailability(m_cups->serviceAvailable());
    }
}

void PrinterWorker::openProperties(quint32 xid, const QString &printerName)
{
    ScpWrapper::openProperties(xid, printerName);
}

void PrinterWorker::openPrintQueue(const QString &printerName)
{
    ScpWrapper::openPrintQueue(printerName);
}

void PrinterWorker::openManagePrinter()
{
    ScpWrapper::openManagePrinter();
}

void PrinterWorker::subscribe()
{
    if (m_cups)
    {
        m_cups->createPrinterSubscription();
    }
}

void PrinterWorker::unsubscribe()
{
    if (m_cups)
    {
        m_cups->cancelPrinterSubscription();
    }
}

void PrinterWorker::refreshPrinters()
{
    if (!m_cups)
    {
        return;
    }
    const QList<PrinterInfo> printers = m_cups->getPrinters();
    // 仅当列表相对上次快照有变化时通知界面
    if (!m_hasSnapshot || printers != m_lastPrinters)
    {
        m_hasSnapshot = true;
        m_lastPrinters = printers;
        emit printersChanged(printers);
    }
}

void PrinterWorker::onPollTimer()
{
    // 仅运行中且服务可用时拉取事件（不可用期间定时器已被停）
    if (!m_running || !m_available || !m_cups)
    {
        return;
    }
    handleDrain(m_cups->drainPrinterNotifications());
}

void PrinterWorker::handleDrain(const CupsWrapper::NotificationDrain &drain)
{
    if (m_timer)
    {
        // 用服务端建议的拉取间隔调整轮询定时器（上限已在 wrapper 内封顶）
        m_timer->setInterval(qMax(1, drain.nextIntervalSec) * 1000);
    }

    using Drain = CupsWrapper::NotificationDrainResult;
    if (drain.result == Drain::NeedResubscribe)
    {
        // 触发重建时旧订阅已不存在（从未建，或服务端已回收置 -1），直接重建，无需先 unsubscribe
        subscribe();
        refreshPrinters();
    }
    else if (drain.result == Drain::HasEvents)
    {
        refreshPrinters();
    }
}

void PrinterWorker::handleServiceAvailableChanged(bool available)
{
    applyAvailability(available);
}

void PrinterWorker::applyAvailability(bool available)
{
    const bool changed = (available != m_available);
    m_available = available;

    // 可用性状态翻转才通知界面，同状态重复调用不刷 UI
    if (changed)
    {
        emit serviceAvailableChanged(available);
    }

    if (!available)
    {
        // 服务不可用：停轮询并取消订阅（服务端订阅随服务停止消失），清空列表快照
        if (m_timer)
        {
            m_timer->stop();
        }
        unsubscribe();
        m_hasSnapshot = false;
        m_lastPrinters.clear();
        return;
    }

    // 刚变为可用（含启动首刷）：建立订阅并取首屏列表
    if (changed && m_cups)
    {
        subscribe();
        refreshPrinters();
    }
    // 服务可用期间保证轮询定时器持续运行
    if (m_timer && !m_timer->isActive())
    {
        m_timer->start();
    }
}
