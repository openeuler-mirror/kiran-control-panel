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

#include <QList>
#include <QObject>

#include "printer-info.h"

class QDBusMessage;

class CupsWrapper : public QObject
{
    Q_OBJECT

public:
    enum class NotificationDrainResult
    {
        NoEvents,
        HasEvents,
        NeedResubscribe,
    };

    struct NotificationDrain
    {
        NotificationDrainResult result = NotificationDrainResult::NoEvents;
        int nextIntervalSec = 1;  // notify-get-interval；缺省与 SCP MIN_REFRESH_INTERVAL 一致
    };

    explicit CupsWrapper(QObject *parent = nullptr);
    ~CupsWrapper() override;

    bool serviceAvailable() const;
    void startServiceWatch();
    void stopServiceWatch();

    QList<PrinterInfo> getPrinters();
    bool setDefaultPrinter(const QString &name);
    bool printTestPage(const QString &name, const QString &title);
    bool startService();

    bool createPrinterSubscription();
    void cancelPrinterSubscription();
    NotificationDrain drainPrinterNotifications();

signals:
    void serviceAvailableChanged(bool available);

private slots:
    void handleUnitPropertiesChanged(const QDBusMessage &message);

private:
    QString resolveCupsUnitPath() const;
    bool readActiveState(const QString &unitPath) const;
    void setServiceAvailable(bool available);

    QString m_unitPath;
    bool m_watching = false;
    bool m_serviceAvailable = false;

    int m_subscriptionId = -1;
    int m_notifySequence = 0;
};
