/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef POWER_H
#define POWER_H

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

struct IdleAction
{
    int idleTimeout;
    int idleAction;
    friend QDBusArgument &operator<<(QDBusArgument &arg, const IdleAction &action)
    {
        arg.beginStructure();
        arg << action.idleTimeout;
        arg << action.idleAction;
        arg.endStructure();
        return arg;
    };
    friend const QDBusArgument &operator>>(const QDBusArgument &arg, IdleAction &action)
    {
        arg.beginStructure();
        arg >> action.idleTimeout;
        arg >> action.idleAction;
        arg.endStructure();
        return arg;
    };
};
Q_DECLARE_METATYPE(IdleAction)

/*
 * Proxy class for interface com.kylinsec.Kiran.SessionDaemon.Power
 */
class PowerInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "com.kylinsec.Kiran.SessionDaemon.Power";
    }

private:
    PowerInterface(const QDBusConnection &connection, QObject *parent = nullptr);

public:
    static PowerInterface *instance();
    ~PowerInterface();

    Q_PROPERTY(int idle_dimmed_scale READ idle_dimmed_scale)
    inline int idle_dimmed_scale() const
    {
        return qvariant_cast<int>(property("idle_dimmed_scale"));
    }

    Q_PROPERTY(bool lid_is_present READ lid_is_present)
    inline bool lid_is_present() const
    {
        return qvariant_cast<bool>(property("lid_is_present"));
    }

    Q_PROPERTY(bool on_battery READ on_battery)
    inline bool on_battery() const
    {
        return qvariant_cast<bool>(property("on_battery"));
    }

public Q_SLOTS:  // METHODS
    inline QDBusPendingReply<int> GetBrightness(int device)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(device);
        return asyncCallWithArgumentList(QStringLiteral("GetBrightness"), argumentList);
    }

    inline QDBusPendingReply<int> GetEventAction(int event)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(event);
        return asyncCallWithArgumentList(QStringLiteral("GetEventAction"), argumentList);
    }

    inline QDBusPendingReply<IdleAction> GetIdleAction(int device, int supply)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(device) << QVariant::fromValue(supply);
        return asyncCallWithArgumentList(QStringLiteral("GetIdleAction"), argumentList);
    }

    inline QDBusPendingReply<> SetBrightness(int device, int brightness_percentage)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(device) << QVariant::fromValue(brightness_percentage);
        return asyncCallWithArgumentList(QStringLiteral("SetBrightness"), argumentList);
    }

    inline QDBusPendingReply<> SetEventAction(int event, int action)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(event) << QVariant::fromValue(action);
        return asyncCallWithArgumentList(QStringLiteral("SetEventAction"), argumentList);
    }

    inline QDBusPendingReply<> SetIdleAction(int device, int supply, int idle_timeout, int action)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(device) << QVariant::fromValue(supply) << QVariant::fromValue(idle_timeout) << QVariant::fromValue(action);
        return asyncCallWithArgumentList(QStringLiteral("SetIdleAction"), argumentList);
    }

    inline QDBusPendingReply<> SetIdleDimmed(int scale)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(scale);
        return asyncCallWithArgumentList(QStringLiteral("SetIdleDimmed"), argumentList);
    }

Q_SIGNALS:  // SIGNALS
    void BrightnessChanged(int device);
    void EventActionChanged(int event);
    void IdleActionChanged(int device, int supply);
};

namespace com
{
namespace kylinsec
{
namespace Kiran
{
namespace SessionDaemon
{
typedef ::PowerInterface Power;
}
}  // namespace Kiran
}  // namespace kylinsec
}  // namespace com
#endif
