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
#include "printer-manager.h"

#include "printer-worker.h"
#include "scp-wrapper.h"

#include <QProcess>
#include <QThread>

PrinterManager::PrinterManager(QObject *parent)
    : QObject(parent)
{
    m_workerThread = new QThread(this);
    m_workerThread->setObjectName(QStringLiteral("printer-worker"));

    m_worker = new PrinterWorker();
    m_worker->moveToThread(m_workerThread);
    // QList<PrinterInfo> 跨线程 queued 传递前必须先注册元类型，否则信号静默丢失（列表不刷新/为空）
    qRegisterMetaType<QList<PrinterInfo>>();
    connect(m_worker, &PrinterWorker::serviceAvailableChanged,
            this, &PrinterManager::serviceStatusChanged);
    connect(m_worker, &PrinterWorker::printersChanged,
            this, &PrinterManager::printersChanged);

    m_workerThread->start();
}

PrinterManager::~PrinterManager()
{
    if (m_addProcess)
    {
        m_addProcess->disconnect(this);
        m_addProcess->deleteLater();
        m_addProcess = nullptr;
    }

    if (m_workerThread && m_worker)
    {
        // 让 worker 在其线程内停止并释放线程内对象（CUPS 包装/定时器），再退出线程
        QMetaObject::invokeMethod(m_worker, "cleanup", Qt::BlockingQueuedConnection);
        m_worker->deleteLater();
        m_workerThread->quit();
        if (!m_workerThread->wait(3000))
        {
            m_workerThread->terminate();
            m_workerThread->wait();
        }
        m_worker = nullptr;
    }
}

void PrinterManager::start()
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "start", Qt::QueuedConnection);
}

void PrinterManager::stop()
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "stop", Qt::QueuedConnection);
}

void PrinterManager::poll()
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "poll", Qt::QueuedConnection);
}

void PrinterManager::setDefault(const QString &printerName)
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "setDefaultPrinter", Qt::QueuedConnection,
                              Q_ARG(QString, printerName));
}

void PrinterManager::printTestPage(const QString &printerName, const QString &title)
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "printTestPage", Qt::QueuedConnection,
                              Q_ARG(QString, printerName), Q_ARG(QString, title));
}

void PrinterManager::startService()
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "startService", Qt::QueuedConnection);
}

bool PrinterManager::openAddDialog(quint32 xid)
{
    Q_UNUSED(xid);

    // 添加向导（helper 进程）在主线程跟踪：结束（完成/取消）后触发后台刷新
    if (m_addProcess && m_addProcess->state() != QProcess::NotRunning)
    {
        return true;
    }
    if (m_addProcess)
    {
        m_addProcess->deleteLater();
        m_addProcess = nullptr;
    }

    QProcess *process = ScpWrapper::startAddPrinterTracked();
    if (!process)
    {
        return false;
    }

    process->setParent(this);
    auto onFinished = [this, process]() {
        poll();
        process->deleteLater();
        m_addProcess = nullptr;
    };
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, onFinished);
    connect(process, &QProcess::errorOccurred, this, onFinished);

    m_addProcess = process;
    return true;
}

void PrinterManager::openProperties(quint32 xid, const QString &printerName)
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "openProperties", Qt::QueuedConnection,
                              Q_ARG(quint32, xid), Q_ARG(QString, printerName));
}

void PrinterManager::openPrintQueue(const QString &printerName)
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "openPrintQueue", Qt::QueuedConnection,
                              Q_ARG(QString, printerName));
}

void PrinterManager::openManagePrinter()
{
    if (!m_worker)
    {
        return;
    }
    QMetaObject::invokeMethod(m_worker, "openManagePrinter", Qt::QueuedConnection);
}
