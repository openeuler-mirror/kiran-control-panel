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

#ifndef AUDIO_STREAM_INTERFACE_H
#define AUDIO_STREAM_INTERFACE_H

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "audio-stream-interface.h"

/*
 * Proxy class for interface com.kylinsec.Kiran.SessionDaemon.Audio.Stream
 */
class AudioStreamInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "com.kylinsec.Kiran.SessionDaemon.Audio.Stream";
    }

public:
    AudioStreamInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~AudioStreamInterface();

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
    inline QDBusPendingReply<QString> GetProperty(const QString &key)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(key);
        return asyncCallWithArgumentList(QStringLiteral("GetProperty"), argumentList);
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
typedef ::AudioStreamInterface Stream;
//        }
}  // namespace SessionDaemon
}  // namespace Kiran
}  // namespace kylinsec
}  // namespace com
#endif
