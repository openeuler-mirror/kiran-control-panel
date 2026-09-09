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
#include "cups-wrapper.h"
#include "logging-category.h"

#include <cups/cups.h>
#include <cups/ipp.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDebug>
#include <QVariantMap>

#include <sys/socket.h>

#include <cstring>

#define CUPS_OPT_PRINTER_INFO "printer-info"
#define CUPS_OPT_MAKE_AND_MODEL "printer-make-and-model"
#define CUPS_OPT_PRINTER_STATE "printer-state"
#define CUPS_TESTPRINT_FILE "/usr/share/cups/data/testprint"

#define SYSTEMD_SERVICE "org.freedesktop.systemd1"
#define SYSTEMD_OBJECT_PATH "/org/freedesktop/systemd1"
#define SYSTEMD_MANAGER_IFACE "org.freedesktop.systemd1.Manager"
#define SYSTEMD_UNIT_IFACE "org.freedesktop.systemd1.Unit"
#define DBUS_PROPERTIES_IFACE "org.freedesktop.DBus.Properties"
#define SYSTEMD_METHOD_GET_UNIT "GetUnit"
#define SYSTEMD_METHOD_LOAD_UNIT "LoadUnit"
#define SYSTEMD_METHOD_START_UNIT "StartUnit"
#define DBUS_METHOD_GET "Get"
#define DBUS_SIGNAL_PROPERTIES_CHANGED "PropertiesChanged"
#define CUPS_UNIT "cups.service"
#define UNIT_PROPERTY_ACTIVE_STATE "ActiveState"
#define UNIT_STATE_ACTIVE "active"
#define START_UNIT_MODE_REPLACE "replace"

#define IPP_URI_ROOT "ipp://localhost/"
#define IPP_ATTR_PRINTER_URI "printer-uri"
#define IPP_ATTR_REQUESTING_USER "requesting-user-name"
#define IPP_ATTR_NOTIFY_EVENTS "notify-events"
#define IPP_ATTR_NOTIFY_PULL_METHOD "notify-pull-method"
#define IPP_ATTR_NOTIFY_LEASE_DURATION "notify-lease-duration"
#define IPP_ATTR_NOTIFY_SUBSCRIPTION_ID "notify-subscription-id"
#define IPP_ATTR_NOTIFY_SUBSCRIPTION_IDS "notify-subscription-ids"
#define IPP_ATTR_NOTIFY_SEQUENCE_NUMBERS "notify-sequence-numbers"
#define IPP_ATTR_NOTIFY_SUBSCRIBED_EVENT "notify-subscribed-event"
#define IPP_ATTR_NOTIFY_GET_INTERVAL "notify-get-interval"
#define IPP_PULL_METHOD_IPPGET "ippget"
#define IPP_EVENT_PRINTER_ADDED "printer-added"
#define IPP_EVENT_PRINTER_DELETED "printer-deleted"
#define IPP_EVENT_PRINTER_MODIFIED "printer-modified"
#define IPP_EVENT_PRINTER_STATE_CHANGED "printer-state-changed"
#define IPP_NOTIFY_LEASE_SECONDS 3600
#define IPP_RESOURCE_ROOT "/"
#define IPP_DEFAULT_NOTIFY_INTERVAL_SEC 1
#define IPP_MAX_NOTIFY_INTERVAL_SEC 5

namespace
{
http_t *openCupsHttp()
{
    return httpConnect2(cupsServer(),
                        ippPort(),
                        nullptr,
                        AF_UNSPEC,
                        cupsEncryption(),
                        1,
                        30000,
                        nullptr);
}

const char *const kPrinterNotifyEvents[] = {
    IPP_EVENT_PRINTER_ADDED,
    IPP_EVENT_PRINTER_DELETED,
    IPP_EVENT_PRINTER_MODIFIED,
    IPP_EVENT_PRINTER_STATE_CHANGED,
};
}  // namespace

CupsWrapper::CupsWrapper(QObject *parent)
    : QObject(parent)
{
}

CupsWrapper::~CupsWrapper()
{
    stopServiceWatch();
    cancelPrinterSubscription();
}

QString CupsWrapper::resolveCupsUnitPath() const
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusMessage getUnit = QDBusMessage::createMethodCall(SYSTEMD_SERVICE,
                                                          SYSTEMD_OBJECT_PATH,
                                                          SYSTEMD_MANAGER_IFACE,
                                                          SYSTEMD_METHOD_GET_UNIT);
    getUnit.setArguments(QVariantList() << QStringLiteral(CUPS_UNIT));
    QDBusReply<QDBusObjectPath> unitReply = bus.call(getUnit, QDBus::Block, 3000);
    if (!unitReply.isValid())
    {
        // GetUnit 对尚未加载的 unit 会失败，退回 LoadUnit 兜底
        QDBusMessage loadUnit = QDBusMessage::createMethodCall(SYSTEMD_SERVICE,
                                                               SYSTEMD_OBJECT_PATH,
                                                               SYSTEMD_MANAGER_IFACE,
                                                               SYSTEMD_METHOD_LOAD_UNIT);
        loadUnit.setArguments(QVariantList() << QStringLiteral(CUPS_UNIT));
        unitReply = bus.call(loadUnit, QDBus::Block, 3000);
        if (!unitReply.isValid())
        {
            KLOG_WARNING(qLcPrinter) << "CupsWrapper::resolveCupsUnitPath GetUnit/LoadUnit failed:"
                                     << unitReply.error();
            return {};
        }
    }
    return unitReply.value().path();
}

bool CupsWrapper::readActiveState(const QString &unitPath) const
{
    if (unitPath.isEmpty())
    {
        return false;
    }

    QDBusMessage getProp = QDBusMessage::createMethodCall(SYSTEMD_SERVICE,
                                                          unitPath,
                                                          DBUS_PROPERTIES_IFACE,
                                                          DBUS_METHOD_GET);
    getProp.setArguments(QVariantList() << QStringLiteral(SYSTEMD_UNIT_IFACE)
                                        << QStringLiteral(UNIT_PROPERTY_ACTIVE_STATE));
    QDBusReply<QDBusVariant> propReply = QDBusConnection::systemBus().call(getProp, QDBus::Block, 3000);
    if (!propReply.isValid())
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::readActiveState Get ActiveState failed:" << propReply.error();
        return false;
    }

    return propReply.value().variant().toString() == QStringLiteral(UNIT_STATE_ACTIVE);
}

void CupsWrapper::setServiceAvailable(bool available)
{
    if (m_serviceAvailable == available)
    {
        return;
    }
    m_serviceAvailable = available;
    emit serviceAvailableChanged(available);
}

bool CupsWrapper::serviceAvailable() const
{
    return m_serviceAvailable;
}

void CupsWrapper::startServiceWatch()
{
    if (m_watching)
    {
        return;
    }

    m_unitPath = resolveCupsUnitPath();
    if (m_unitPath.isEmpty())
    {
        // 解析不到 cups.service（systemd 不可用）按服务不可用处理
        setServiceAvailable(false);
        return;
    }

    setServiceAvailable(readActiveState(m_unitPath));

    const bool ok = QDBusConnection::systemBus().connect(QStringLiteral(SYSTEMD_SERVICE),
                                                         m_unitPath,
                                                         QStringLiteral(DBUS_PROPERTIES_IFACE),
                                                         QStringLiteral(DBUS_SIGNAL_PROPERTIES_CHANGED),
                                                         QStringLiteral("sa{sv}as"),
                                                         this,
                                                         SLOT(handleUnitPropertiesChanged(QDBusMessage)));
    if (!ok)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::startServiceWatch: connect PropertiesChanged failed for"
                                 << m_unitPath;
        return;
    }
    m_watching = true;
}

void CupsWrapper::stopServiceWatch()
{
    if (!m_watching)
    {
        return;
    }

    QDBusConnection::systemBus().disconnect(QStringLiteral(SYSTEMD_SERVICE),
                                            m_unitPath,
                                            QStringLiteral(DBUS_PROPERTIES_IFACE),
                                            QStringLiteral(DBUS_SIGNAL_PROPERTIES_CHANGED),
                                            QStringLiteral("sa{sv}as"),
                                            this,
                                            SLOT(handleUnitPropertiesChanged(QDBusMessage)));
    m_watching = false;
}

void CupsWrapper::handleUnitPropertiesChanged(const QDBusMessage &message)
{
    const QList<QVariant> args = message.arguments();
    if (args.size() < 2)
    {
        return;
    }

    const QString iface = args.at(0).toString();
    if (iface != QStringLiteral(SYSTEMD_UNIT_IFACE))
    {
        return;
    }

    const QVariantMap changed = qdbus_cast<QVariantMap>(args.at(1));
    // 只关心 ActiveState 变化，其它属性（如 SubState）不影响可用性判断
    if (!changed.contains(QStringLiteral(UNIT_PROPERTY_ACTIVE_STATE)))
    {
        return;
    }

    const QString state = changed.value(QStringLiteral(UNIT_PROPERTY_ACTIVE_STATE)).toString();
    setServiceAvailable(state == QStringLiteral(UNIT_STATE_ACTIVE));
}

QList<PrinterInfo> CupsWrapper::getPrinters()
{
    QList<PrinterInfo> printers;

    cups_dest_t *dests = NULL;
    int numDests = cupsGetDests(&dests);
    if (numDests < 0)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::getPrinters: cupsGetDests failed:" << cupsLastErrorString();
        return printers;
    }

    for (int i = 0; i < numDests; ++i)
    {
        const cups_dest_t &dest = dests[i];

        PrinterInfo info;
        info.name = QString::fromUtf8(dest.name);
        info.isDefault = (dest.is_default != 0);
        info.isOnline = false;

        const char *printerInfo = cupsGetOption(CUPS_OPT_PRINTER_INFO, dest.num_options, dest.options);
        const char *makeAndModel = cupsGetOption(CUPS_OPT_MAKE_AND_MODEL, dest.num_options, dest.options);
        if (printerInfo && printerInfo[0] != '\0')
        {
            info.model = QString::fromUtf8(printerInfo);
        }
        else if (makeAndModel)
        {
            info.model = QString::fromUtf8(makeAndModel);
        }

        const char *stateOpt = cupsGetOption(CUPS_OPT_PRINTER_STATE, dest.num_options, dest.options);
        if (stateOpt && stateOpt[0] != '\0')
        {
            // IPP printer-state：3=空闲、4=处理中 视为可用的在线打印机（5=停止/错误 视为离线）
            const int state = atoi(stateOpt);
            info.isOnline = (state == IPP_PSTATE_IDLE || state == IPP_PSTATE_PROCESSING);
        }

        printers.append(info);
    }

    cupsFreeDests(numDests, dests);
    return printers;
}

bool CupsWrapper::setDefaultPrinter(const QString &name)
{
    QByteArray nameUtf8 = name.toUtf8();
    cups_dest_t *dests = NULL;
    int numDests = cupsGetDests(&dests);
    if (numDests < 0)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::setDefaultPrinter: cupsGetDests failed:" << cupsLastErrorString();
        return false;
    }

    // cupsSetDefaultDest 只改内存；必须再 cupsSetDests 才写入 ~/.cups/lpoptions
    cupsSetDefaultDest(nameUtf8.constData(), NULL, numDests, dests);
    cupsSetDests(numDests, dests);
    cupsFreeDests(numDests, dests);

    cups_dest_t *check = NULL;
    int n = cupsGetDests(&check);
    bool ok = false;
    if (n >= 0)
    {
        cups_dest_t *dest = cupsGetDest(nameUtf8.constData(), NULL, n, check);
        ok = (dest && dest->is_default);
        cupsFreeDests(n, check);
    }
    if (!ok)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::setDefaultPrinter: persist failed for" << name
                                 << cupsLastErrorString();
    }
    return ok;
}

bool CupsWrapper::printTestPage(const QString &name, const QString &title)
{
    QByteArray nameUtf8 = name.toUtf8();
    QByteArray titleUtf8 = title.toUtf8();
    int jobId = cupsPrintFile(nameUtf8.constData(),
                              CUPS_TESTPRINT_FILE,
                              titleUtf8.constData(),
                              0,
                              NULL);
    if (jobId <= 0)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::printTestPage failed:" << cupsLastErrorString();
        return false;
    }
    return true;
}

bool CupsWrapper::startService()
{
    QDBusMessage message = QDBusMessage::createMethodCall(SYSTEMD_SERVICE,
                                                          SYSTEMD_OBJECT_PATH,
                                                          SYSTEMD_MANAGER_IFACE,
                                                          SYSTEMD_METHOD_START_UNIT);
    message.setArguments(QVariantList() << QStringLiteral(CUPS_UNIT) << QStringLiteral(START_UNIT_MODE_REPLACE));
    QDBusReply<QDBusObjectPath> reply = QDBusConnection::systemBus().call(message, QDBus::Block, 10000);
    if (!reply.isValid())
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::startService failed:" << reply.error();
        return false;
    }
    return true;
}

bool CupsWrapper::createPrinterSubscription()
{
    // 已有有效订阅则直接复用，避免反复重建；订阅失效（drain 检测到过期）会把 id 置 -1
    if (m_subscriptionId > 0)
    {
        return true;
    }

    http_t *http = openCupsHttp();
    if (!http)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::createPrinterSubscription: httpConnect2 failed";
        return false;
    }

    ipp_t *request = ippNewRequest(IPP_OP_CREATE_PRINTER_SUBSCRIPTIONS);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, IPP_ATTR_PRINTER_URI, nullptr, IPP_URI_ROOT);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, IPP_ATTR_REQUESTING_USER, nullptr, cupsUser());
    ippAddStrings(request,
                  IPP_TAG_SUBSCRIPTION,
                  IPP_TAG_KEYWORD,
                  IPP_ATTR_NOTIFY_EVENTS,
                  static_cast<int>(sizeof(kPrinterNotifyEvents) / sizeof(kPrinterNotifyEvents[0])),
                  nullptr,
                  kPrinterNotifyEvents);
    ippAddString(request, IPP_TAG_SUBSCRIPTION, IPP_TAG_KEYWORD, IPP_ATTR_NOTIFY_PULL_METHOD, nullptr, IPP_PULL_METHOD_IPPGET);
    ippAddInteger(request, IPP_TAG_SUBSCRIPTION, IPP_TAG_INTEGER, IPP_ATTR_NOTIFY_LEASE_DURATION, IPP_NOTIFY_LEASE_SECONDS);

    // cupsDoRequest will free the request
    ipp_t *response = cupsDoRequest(http, request, IPP_RESOURCE_ROOT);
    if (!response || cupsLastError() >= IPP_STATUS_ERROR_BAD_REQUEST)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::createPrinterSubscription failed:" << cupsLastErrorString();
        ippDelete(response);
        httpClose(http);
        return false;
    }

    ipp_attribute_t *attr = ippFindAttribute(response, IPP_ATTR_NOTIFY_SUBSCRIPTION_ID, IPP_TAG_INTEGER);
    if (!attr)
    {
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::createPrinterSubscription: missing notify-subscription-id";
        ippDelete(response);
        httpClose(http);
        return false;
    }

    m_subscriptionId = ippGetInteger(attr, 0);
    m_notifySequence = 0;
    ippDelete(response);
    httpClose(http);
    return m_subscriptionId > 0;
}

void CupsWrapper::cancelPrinterSubscription()
{
    // 仅服务停止/退出路径调用；取消请求失败也清 id，避免残留 id 使后续 subscribe 被复用守卫直接跳过
    if (m_subscriptionId <= 0)
    {
        m_subscriptionId = -1;
        m_notifySequence = 0;
        return;
    }

    http_t *http = openCupsHttp();
    if (http)
    {
        ipp_t *request = ippNewRequest(IPP_OP_CANCEL_SUBSCRIPTION);
        ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, IPP_ATTR_PRINTER_URI, nullptr, IPP_URI_ROOT);
        ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, IPP_ATTR_REQUESTING_USER, nullptr, cupsUser());
        ippAddInteger(request, IPP_TAG_OPERATION, IPP_TAG_INTEGER, IPP_ATTR_NOTIFY_SUBSCRIPTION_ID, m_subscriptionId);

        // cupsDoRequest will free the request
        ipp_t *response = cupsDoRequest(http, request, IPP_RESOURCE_ROOT);
        if (!response || cupsLastError() >= IPP_STATUS_ERROR_BAD_REQUEST)
        {
            KLOG_WARNING(qLcPrinter) << "CupsWrapper::cancelPrinterSubscription failed:" << cupsLastErrorString();
        }
        ippDelete(response);
        httpClose(http);
    }

    m_subscriptionId = -1;
    m_notifySequence = 0;
}

CupsWrapper::NotificationDrain CupsWrapper::drainPrinterNotifications()
{
    NotificationDrain drain;
    drain.nextIntervalSec = IPP_DEFAULT_NOTIFY_INTERVAL_SEC;

    // 无有效订阅（从未建 / 已被置 -1 判定失效）：需要先重建订阅
    if (m_subscriptionId <= 0)
    {
        drain.result = NotificationDrainResult::NeedResubscribe;
        return drain;
    }

    http_t *http = openCupsHttp();
    if (!http)
    {
        // 连接失败按无事件处理：保留订阅 id，下一轮重试
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::drainPrinterNotifications: httpConnect2 failed";
        drain.result = NotificationDrainResult::NoEvents;
        return drain;
    }

    ipp_t *request = ippNewRequest(IPP_OP_GET_NOTIFICATIONS);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, IPP_ATTR_PRINTER_URI, nullptr, IPP_URI_ROOT);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_NAME, IPP_ATTR_REQUESTING_USER, nullptr, cupsUser());
    ippAddInteger(request, IPP_TAG_OPERATION, IPP_TAG_INTEGER, IPP_ATTR_NOTIFY_SUBSCRIPTION_IDS, m_subscriptionId);
    if (m_notifySequence > 0)
    {
        ippAddInteger(request, IPP_TAG_OPERATION, IPP_TAG_INTEGER, IPP_ATTR_NOTIFY_SEQUENCE_NUMBERS, m_notifySequence + 1);
    }

    // cupsDoRequest will free the request
    ipp_t *response = cupsDoRequest(http, request, IPP_RESOURCE_ROOT);
    const ipp_status_t status = cupsLastError();
    if (!response || status >= IPP_STATUS_ERROR_BAD_REQUEST)
    {
        const QString err = QString::fromUtf8(cupsLastErrorString());
        ippDelete(response);
        httpClose(http);
        if (status == IPP_STATUS_ERROR_NOT_FOUND)
        {
            // 订阅已被服务端回收（租约到期/服务重启）：置 -1 交由上层重建，无需再发 Cancel
            KLOG_WARNING(qLcPrinter) << "CupsWrapper::drainPrinterNotifications: subscription expired";
            m_subscriptionId = -1;
            m_notifySequence = 0;
            drain.result = NotificationDrainResult::NeedResubscribe;
            return drain;
        }
        // 其它错误：保留现有订阅下轮重试（仅 not-found 才判定失效并重建）
        KLOG_WARNING(qLcPrinter) << "CupsWrapper::drainPrinterNotifications failed:" << err;
        drain.result = NotificationDrainResult::NoEvents;
        return drain;
    }

    ipp_attribute_t *intervalAttr = ippFindAttribute(response, IPP_ATTR_NOTIFY_GET_INTERVAL, IPP_TAG_INTEGER);
    if (intervalAttr)
    {
        const int interval = ippGetInteger(intervalAttr, 0);
        if (interval > 0)
        {
            // CUPS 可能给很大间隔（实测 60s），封顶以免事件驱动刷新过慢
            drain.nextIntervalSec = qMin(interval, IPP_MAX_NOTIFY_INTERVAL_SEC);
        }
    }

    bool hasPrinterEvent = false;
    for (ipp_attribute_t *attr = ippFirstAttribute(response); attr; attr = ippNextAttribute(response))
    {
        if (ippGetGroupTag(attr) != IPP_TAG_EVENT_NOTIFICATION)
        {
            continue;
        }

        const char *name = ippGetName(attr);
        if (!name)
        {
            continue;
        }

        if (strcmp(name, "notify-sequence-number") == 0 && ippGetValueTag(attr) == IPP_TAG_INTEGER)
        {
            const int seq = ippGetInteger(attr, 0);
            if (seq > m_notifySequence)
            {
                m_notifySequence = seq;
            }
        }
        else if (strcmp(name, IPP_ATTR_NOTIFY_SUBSCRIBED_EVENT) == 0)
        {
            const char *event = ippGetString(attr, 0, nullptr);
            // 仅 printer-* 事件（added/deleted/modified/state-changed）需要刷新列表
            if (event && strncmp(event, "printer-", 8) == 0)
            {
                hasPrinterEvent = true;
            }
        }
    }

    ippDelete(response);
    httpClose(http);
    drain.result = hasPrinterEvent ? NotificationDrainResult::HasEvents : NotificationDrainResult::NoEvents;
    return drain;
}
