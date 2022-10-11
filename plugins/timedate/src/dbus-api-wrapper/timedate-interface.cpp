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

#include "timedate-interface.h"
#include <QMutex>
#include <QScopedPointer>
#include <QDBusConnection>


ComKylinsecKiranSystemDaemonTimeDateInterface *ComKylinsecKiranSystemDaemonTimeDateInterface::instance()
{
    static QMutex mutex;
    static QScopedPointer<ComKylinsecKiranSystemDaemonTimeDateInterface> pInst;

    if(Q_UNLIKELY(!pInst)){
        QMutexLocker locker(&mutex);
        if(pInst.isNull()){
            pInst.reset(new ComKylinsecKiranSystemDaemonTimeDateInterface("com.kylinsec.Kiran.SystemDaemon",
                                                                          "/com/kylinsec/Kiran/SystemDaemon/TimeDate",
                                                                          QDBusConnection::systemBus()));
        }
    }

    return pInst.data();
}

/*
 * Implementation of interface class ComKylinsecKiranSystemDaemonTimeDateInterface
 */
ComKylinsecKiranSystemDaemonTimeDateInterface::ComKylinsecKiranSystemDaemonTimeDateInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    qRegisterMetaType<ZoneInfo>("ZoneInfo");
    qDBusRegisterMetaType<ZoneInfo>();
    qRegisterMetaType<TimeZoneList>("TimeZoneList");
    qDBusRegisterMetaType<TimeZoneList>();

    ///获取属性变更,属性变更发送信号
    QDBusConnection::systemBus().connect("com.kylinsec.Kiran.SystemDaemon",
                                         "/com/kylinsec/Kiran/SystemDaemon/TimeDate",
                                         "org.freedesktop.DBus.Properties",
                                         "PropertiesChanged",this,SLOT(handlePropertiesChanged(QDBusMessage)));

}

ComKylinsecKiranSystemDaemonTimeDateInterface::~ComKylinsecKiranSystemDaemonTimeDateInterface()
{

}

void ComKylinsecKiranSystemDaemonTimeDateInterface::handlePropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    for(auto iter=changedProps.begin();iter!=changedProps.end();iter++){
        qInfo() << iter.key() << iter.value();
        emit propertyChanged(iter.key(),iter.value());
    }
}
