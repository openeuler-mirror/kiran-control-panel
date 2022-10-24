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

#include "audio-device-interface.h"

/*
 * Implementation of interface class AudioDeviceInterface
 */

AudioDeviceInterface::AudioDeviceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    const_cast<QDBusConnection &>(connection).connect(service, path, "org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as", this, SLOT(handleDbusPropertyChanged(QDBusMessage)));
}

AudioDeviceInterface::~AudioDeviceInterface()
{
}

void sendPropertyChangedDetailSignal(AudioDeviceInterface *ptr, const QString &propertyName, QVariant value)
{
    if (propertyName == QStringLiteral("active_port"))
    {
        const QString &active_portValue = qvariant_cast<QString>(value);
        Q_EMIT ptr->active_portChanged(active_portValue);
        return;
    }

    if (propertyName == QStringLiteral("balance"))
    {
        const double &balanceValue = qvariant_cast<double>(value);
        Q_EMIT ptr->balanceChanged(balanceValue);
        return;
    }

    if (propertyName == QStringLiteral("base_volume"))
    {
        const double &base_volumeValue = qvariant_cast<double>(value);
        Q_EMIT ptr->base_volumeChanged(base_volumeValue);
        return;
    }

    if (propertyName == QStringLiteral("card_index"))
    {
        const uint &card_indexValue = qvariant_cast<uint>(value);
        Q_EMIT ptr->card_indexChanged(card_indexValue);
        return;
    }

    if (propertyName == QStringLiteral("fade"))
    {
        const double &fadeValue = qvariant_cast<double>(value);
        Q_EMIT ptr->fadeChanged(fadeValue);
        return;
    }

    if (propertyName == QStringLiteral("index"))
    {
        const uint &indexValue = qvariant_cast<uint>(value);
        Q_EMIT ptr->indexChanged(indexValue);
        return;
    }

    if (propertyName == QStringLiteral("mute"))
    {
        const bool &muteValue = qvariant_cast<bool>(value);
        Q_EMIT ptr->muteChanged(muteValue);
        return;
    }

    if (propertyName == QStringLiteral("name"))
    {
        const QString &nameValue = qvariant_cast<QString>(value);
        Q_EMIT ptr->nameChanged(nameValue);
        return;
    }

    if (propertyName == QStringLiteral("state"))
    {
        const uint &stateValue = qvariant_cast<uint>(value);
        Q_EMIT ptr->stateChanged(stateValue);
        return;
    }

    if (propertyName == QStringLiteral("volume"))
    {
        const double &volumeValue = qvariant_cast<double>(value);
        Q_EMIT ptr->volumeChanged(volumeValue);
        return;
    }

    qWarning() << "property not handle: " << propertyName;
    return;
}

void AudioDeviceInterface::handleDbusPropertyChanged(const QDBusMessage &message)
{
    QList<QVariant> args = message.arguments();
    if (args.count() != 3)
        return;
    QString interfaceName = args.at(0).toString();
    if (interfaceName != interface())
        return;
    QVariantMap changedProperties = qdbus_cast<QVariantMap>(args.at(1).value<QDBusArgument>());
    const QMetaObject *metaObj = metaObject();
    for (auto iter = changedProperties.begin(); iter != changedProperties.end(); iter++)
    {
        QString property = iter.key();
        QVariant propertyValue = iter.value();
        const char *propertySignature = QDBusMetaType::typeToSignature(propertyValue.userType());
        int propertyIdx = metaObj->indexOfProperty(property.toLatin1().constData());
        if (propertyIdx == -1)
            continue;
        QMetaProperty metaProperty = metaObj->property(propertyIdx);
        const char *metaPropertySignature = QDBusMetaType::typeToSignature(metaProperty.userType());
        QMetaMethod notifySignal = metaProperty.notifySignal();
        if (!notifySignal.isValid())
            continue;
        // property type is the same as the meta property property
        if (metaProperty.userType() == propertyValue.userType())
        {
            Q_EMIT dbusPropertyChanged(property, propertyValue);
            sendPropertyChangedDetailSignal(this, property, propertyValue);
            continue;
        }
        QDBusArgument dbusArg = propertyValue.value<QDBusArgument>();
        // property type different from meta property property and can't demarshalling
        if (propertyValue.userType() != qMetaTypeId<QDBusArgument>() && metaPropertySignature != dbusArg.currentSignature().toLatin1())
        {
            QString errmsg = QString("Unexpected `%1' (%2)"
                                     "when retrieving property '%3.%4'(expected type '%5' (%6)");
            errmsg = errmsg.arg(propertyValue.typeName(), propertySignature, interface(), metaProperty.name(), metaProperty.typeName(), metaPropertySignature);
            qWarning() << errmsg;
            continue;
        }
        // demarshalling dbus argument
        QVariant result;
        QDBusMetaType::demarshall(dbusArg, metaProperty.userType(), result.data());
        if (!result.isValid())
        {
            QString errmsg = QString("Unexpected `%1' (%2)"
                                     "when retrieving property '%3.%4'(expected type '%5' (%6)");
            errmsg = errmsg.arg(propertyValue.typeName(), propertySignature, interface(), metaProperty.name(), metaProperty.typeName(), metaPropertySignature);
            qWarning() << errmsg;
            continue;
        }
        Q_EMIT dbusPropertyChanged(property, result);
        sendPropertyChangedDetailSignal(this, property, result);
    }
}
