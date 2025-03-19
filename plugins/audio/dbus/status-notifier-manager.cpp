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

#include "status-notifier-manager.h"

/*
 * Implementation of interface class StatusNotifierManagerInterface
 */

StatusNotifierManagerInterface::StatusNotifierManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    const_cast<QDBusConnection &>(connection).connect(service, path, "org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as", this, SLOT(handleDbusPropertyChanged(QDBusMessage)));
}

StatusNotifierManagerInterface::~StatusNotifierManagerInterface()
{
}

void sendPropertyChangedDetailSignal(StatusNotifierManagerInterface *ptr, const QString &propertyName, QVariant value)
{
    if (propertyName == QStringLiteral("icon_size"))
    {
        const int &icon_sizeValue = qvariant_cast<int>(value);
        Q_EMIT ptr->icon_sizeChanged(icon_sizeValue);
        return;
    }

    qWarning() << "property not handle: " << propertyName;
    return;
}

void StatusNotifierManagerInterface::handleDbusPropertyChanged(const QDBusMessage &message)
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
