/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-audio is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#ifndef AUDIO_DEVICE_INTERFACE_H
#define AUDIO_DEVICE_INTERFACE_H

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "audio-device-interface.h"

/*
 * Proxy class for interface com.kylinsec.Kiran.SessionDaemon.Audio.Device
 */
class AudioDeviceInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "com.kylinsec.Kiran.SessionDaemon.Audio.Device";
    }

public:
    AudioDeviceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~AudioDeviceInterface();

    Q_PROPERTY(QString active_port READ active_port NOTIFY active_portChanged)
    inline QString active_port() const
    {
        return qvariant_cast<QString>(property("active_port"));
    }

    Q_PROPERTY(double balance READ balance NOTIFY balanceChanged)
    inline double balance() const
    {
        return qvariant_cast<double>(property("balance"));
    }

    Q_PROPERTY(double base_volume READ base_volume NOTIFY base_volumeChanged)
    inline double base_volume() const
    {
        return qvariant_cast<double>(property("base_volume"));
    }

    Q_PROPERTY(uint card_index READ card_index NOTIFY card_indexChanged)
    inline uint card_index() const
    {
        return qvariant_cast<uint>(property("card_index"));
    }

    Q_PROPERTY(double fade READ fade NOTIFY fadeChanged)
    inline double fade() const
    {
        return qvariant_cast<double>(property("fade"));
    }

    Q_PROPERTY(uint index READ index NOTIFY indexChanged)
    inline uint index() const
    {
        return qvariant_cast<uint>(property("index"));
    }

    Q_PROPERTY(bool mute READ mute NOTIFY muteChanged)
    inline bool mute() const
    {
        return qvariant_cast<bool>(property("mute"));
    }

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    inline QString name() const
    {
        return qvariant_cast<QString>(property("name"));
    }

    Q_PROPERTY(uint state READ state NOTIFY stateChanged)
    inline uint state() const
    {
        return qvariant_cast<uint>(property("state"));
    }

    Q_PROPERTY(double volume READ volume NOTIFY volumeChanged)
    inline double volume() const
    {
        return qvariant_cast<double>(property("volume"));
    }

public Q_SLOTS:  // METHODS
    inline QDBusPendingReply<QString> GetPorts()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetPorts"), argumentList);
    }

    inline QDBusPendingReply<QString> GetProperty(const QString &key)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(key);
        return asyncCallWithArgumentList(QStringLiteral("GetProperty"), argumentList);
    }

    inline QDBusPendingReply<> SetActivePort(const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(name);
        return asyncCallWithArgumentList(QStringLiteral("SetActivePort"), argumentList);
    }

    inline QDBusPendingReply<> SetBalance(double balance)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(balance);
        return asyncCallWithArgumentList(QStringLiteral("SetBalance"), argumentList);
    }

    inline QDBusPendingReply<> SetFade(double fade)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(fade);
        return asyncCallWithArgumentList(QStringLiteral("SetFade"), argumentList);
    }

    inline QDBusPendingReply<> SetMute(bool mute)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(mute);
        return asyncCallWithArgumentList(QStringLiteral("SetMute"), argumentList);
    }

    inline QDBusPendingReply<> SetVolume(double volume)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(volume);
        return asyncCallWithArgumentList(QStringLiteral("SetVolume"), argumentList);
    }

Q_SIGNALS:  // SIGNALS

Q_SIGNALS:  // PROPERTY CHANGED SIGNALS
    void dbusPropertyChanged(const QString &name, const QVariant &value);
    void active_portChanged(const QString &value) const;
    void balanceChanged(double value) const;
    void base_volumeChanged(double value) const;
    void card_indexChanged(uint value) const;
    void fadeChanged(double value) const;
    void indexChanged(uint value) const;
    void muteChanged(bool value) const;
    void nameChanged(const QString &value) const;
    void stateChanged(uint value) const;
    void volumeChanged(double value) const;

private Q_SLOTS:  //PRIVATE PROPERTY CHANGED SLOT
    void handleDbusPropertyChanged(const QDBusMessage &message);
};

namespace com
{
namespace kylinsec
{
namespace Kiran
{
namespace SessionDaemon
{
//        namespace Audio {
typedef ::AudioDeviceInterface Device;
//        }
}  // namespace SessionDaemon
}  // namespace Kiran
}  // namespace kylinsec
}  // namespace com
#endif
