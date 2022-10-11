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

#ifndef AUDIO_INTERFACE_H
#define AUDIO_INTERFACE_H

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "audio-interface.h"

/*
 * Proxy class for interface com.kylinsec.Kiran.SessionDaemon.Audio
 */
class AudioInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "com.kylinsec.Kiran.SessionDaemon.Audio";
    }

private:
    AudioInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

public:
    static AudioInterface *instance();
    ~AudioInterface();

    Q_PROPERTY(uint state READ state NOTIFY stateChanged)
    inline uint state() const
    {
        return qvariant_cast<uint>(property("state"));
    }

public Q_SLOTS:  // METHODS
    inline QDBusPendingReply<QString> GetDefaultSink()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetDefaultSink"), argumentList);
    }

    inline QDBusPendingReply<QString> GetDefaultSource()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetDefaultSource"), argumentList);
    }

    inline QDBusPendingReply<QString> GetSink(uint index)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(index);
        return asyncCallWithArgumentList(QStringLiteral("GetSink"), argumentList);
    }

    inline QDBusPendingReply<QString> GetSinkInput(uint index)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(index);
        return asyncCallWithArgumentList(QStringLiteral("GetSinkInput"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetSinkInputs()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetSinkInputs"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetSinks()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetSinks"), argumentList);
    }

    inline QDBusPendingReply<QString> GetSource(uint index)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(index);
        return asyncCallWithArgumentList(QStringLiteral("GetSource"), argumentList);
    }

    inline QDBusPendingReply<QString> GetSourceOutput(uint index)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(index);
        return asyncCallWithArgumentList(QStringLiteral("GetSourceOutput"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetSourceOutputs()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetSourceOutputs"), argumentList);
    }

    inline QDBusPendingReply<QStringList> GetSources()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetSources"), argumentList);
    }

    inline QDBusPendingReply<> SetDefaultSink(uint sink_index)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(sink_index);
        return asyncCallWithArgumentList(QStringLiteral("SetDefaultSink"), argumentList);
    }

    inline QDBusPendingReply<> SetDefaultSource(uint source_index)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(source_index);
        return asyncCallWithArgumentList(QStringLiteral("SetDefaultSource"), argumentList);
    }

Q_SIGNALS:  // SIGNALS
    void DefaultSinkChange(uint index);
    void DefaultSourceChange(uint index);
    void SinkAdded(uint index);
    void SinkDelete(uint index);
    void SinkInputAdded(uint index);
    void SinkInputDelete(uint index);
    void SourceAdded(uint index);
    void SourceDelete(uint index);
    void SourceOutputAdded(uint index);
    void SourceOutputDelete(uint index);

Q_SIGNALS:  // PROPERTY CHANGED SIGNALS
    void dbusPropertyChanged(const QString &name, const QVariant &value);
    void stateChanged(uint value) const;

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
typedef ::AudioInterface Audio;
}
}  // namespace Kiran
}  // namespace kylinsec
}  // namespace com
#endif
