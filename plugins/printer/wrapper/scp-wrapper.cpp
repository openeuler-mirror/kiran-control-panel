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
#include "scp-wrapper.h"
#include "logging-category.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFile>
#include <QProcess>
#include <QStringList>

#define SCP_DBUS_NAME "org.fedoraproject.Config.Printing"
#define SCP_DBUS_PATH "/org/fedoraproject/Config/Printing"
#define SCP_DBUS_IFACE "org.fedoraproject.Config.Printing"
#define SCP_METHOD_PRINTER_PROPERTIES "PrinterPropertiesDialog"

#define SCP_BIN "/usr/bin/system-config-printer"
#define SCP_ARG_SHOW_JOBS "--show-jobs"
#define SCP_NEWPRINTER_PY "/usr/share/system-config-printer/newprinter.py"
#ifndef KCP_OPEN_NEW_PRINTER_HELPER
#define KCP_OPEN_NEW_PRINTER_HELPER "/usr/libexec/kiran-cpanel-open-new-printer"
#endif

namespace
{
bool addPrinterLaunchable()
{
    if (!QFile::exists(QStringLiteral(KCP_OPEN_NEW_PRINTER_HELPER)))
    {
        KLOG_WARNING(qLcPrinter) << "ScpWrapper::openAddPrinter: helper not found"
                                 << KCP_OPEN_NEW_PRINTER_HELPER;
        return false;
    }
    if (!QFile::exists(QStringLiteral(SCP_NEWPRINTER_PY)))
    {
        KLOG_WARNING(qLcPrinter) << "ScpWrapper::openAddPrinter:" << SCP_NEWPRINTER_PY << "not found";
        return false;
    }
    return true;
}

bool startScp(const QStringList &args)
{
    if (!QFile::exists(QStringLiteral(SCP_BIN)))
    {
        KLOG_WARNING(qLcPrinter) << "ScpWrapper:" << SCP_BIN << "not found";
        return false;
    }

    const bool ok = QProcess::startDetached(QStringLiteral(SCP_BIN), args);
    if (!ok)
    {
        KLOG_WARNING(qLcPrinter) << "ScpWrapper: failed to start" << SCP_BIN;
    }
    return ok;
}
}  // namespace

bool ScpWrapper::openProperties(quint32 xid, const QString &printerName)
{
    QDBusMessage message = QDBusMessage::createMethodCall(SCP_DBUS_NAME,
                                                          SCP_DBUS_PATH,
                                                          SCP_DBUS_IFACE,
                                                          SCP_METHOD_PRINTER_PROPERTIES);
    message.setArguments(QVariantList() << xid << printerName);
    QDBusReply<QString> reply = QDBusConnection::sessionBus().call(message, QDBus::Block, 10000);
    if (!reply.isValid())
    {
        KLOG_WARNING(qLcPrinter) << "ScpWrapper::openProperties failed:" << reply.error();
        return false;
    }
    return true;
}

bool ScpWrapper::openPrintQueue(const QString &printerName)
{
    if (printerName.isEmpty())
    {
        KLOG_WARNING(qLcPrinter) << "ScpWrapper::openPrintQueue: empty printer name";
        return false;
    }

    return startScp(QStringList() << QStringLiteral(SCP_ARG_SHOW_JOBS) << printerName);
}

bool ScpWrapper::openManagePrinter()
{
    return startScp(QStringList());
}

bool ScpWrapper::openAddPrinter()
{
    if (!addPrinterLaunchable())
    {
        return false;
    }

    const bool ok = QProcess::startDetached(QStringLiteral(KCP_OPEN_NEW_PRINTER_HELPER), QStringList());
    if (!ok)
    {
        KLOG_WARNING(qLcPrinter) << "ScpWrapper::openAddPrinter: failed to start" << KCP_OPEN_NEW_PRINTER_HELPER;
    }
    return ok;
}

QProcess *ScpWrapper::startAddPrinterTracked()
{
    if (!addPrinterLaunchable())
    {
        return nullptr;
    }

    auto *process = new QProcess();
    process->setProgram(QStringLiteral(KCP_OPEN_NEW_PRINTER_HELPER));
    process->start();
    return process;
}
