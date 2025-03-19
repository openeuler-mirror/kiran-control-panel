/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#ifndef STATUS_NOTIFIER_MANAGER_H
#define STATUS_NOTIFIER_MANAGER_H

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "status-notifier-manager.h"

/*
 * Proxy class for interface org.kde.StatusNotifierManager
 */
class StatusNotifierManagerInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "org.kde.StatusNotifierManager";
    }

public:
    StatusNotifierManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~StatusNotifierManagerInterface();

    Q_PROPERTY(int icon_size READ icon_size NOTIFY icon_sizeChanged)
    inline int icon_size() const
    {
        return qvariant_cast<int>(property("icon_size"));
    }

public Q_SLOTS:  // METHODS
    inline QDBusPendingReply<QString> GetGeometry(const QString &id)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(id);
        return asyncCallWithArgumentList(QStringLiteral("GetGeometry"), argumentList);
    }

    inline QDBusPendingReply<QString> GetStyle()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetStyle"), argumentList);
    }

Q_SIGNALS:  // SIGNALS
    void GeometryChanged(const QString &id, const QString &geometry);
    void PointClicked(const QString &id, int event_type, int x, int y);
    void StyleChanged(const QString &style);

Q_SIGNALS:  // PROPERTY CHANGED SIGNALS
    void dbusPropertyChanged(const QString &name, const QVariant &value);
    void icon_sizeChanged(int value) const;

private Q_SLOTS:  //PRIVATE PROPERTY CHANGED SLOT
    void handleDbusPropertyChanged(const QDBusMessage &message);
};

namespace org
{
namespace kde
{
typedef ::StatusNotifierManagerInterface StatusNotifierManager;
}
}  // namespace org
#endif
