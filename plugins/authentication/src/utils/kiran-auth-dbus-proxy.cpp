/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#include "kiran-auth-dbus-proxy.h"
#include "auxiliary.h"
#include "kas_auth_proxy.h"
#include "kas_auth_user_proxy.h"

#include <qt5-log-i.h>
#include <sys/types.h>
#include <unistd.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

KiranAuthDBusProxy* KiranAuthDBusProxy::m_instance = nullptr;

void KiranAuthDBusProxy::globalInit()
{
    m_instance = new KiranAuthDBusProxy();
    m_instance->init();
}

KiranAuthDBusProxy::~KiranAuthDBusProxy()
{
}

bool KiranAuthDBusProxy::getAuthTypeEnabled(KADAuthType type)
{
    auto reply = m_authProxy->GetAuthTypeEnabled(type);
    return reply.value();
}

void KiranAuthDBusProxy::setAuthTypeEnabled(KADAuthType type,
                                            bool enabled)
{
    auto reply = m_authProxy->SetAuthTypeEnabled(type, enabled);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "set auth type" << type << "enabled" << enabled << "failed!";
    }
}

bool KiranAuthDBusProxy::getAuthTypeEnabledForApp(KADAuthType authType,
                                                  KADAuthApplication authApp)
{
    auto reply = m_authProxy->GetAuthTypeEnabledForApp(authType, authApp);
    return reply.value();
}

void KiranAuthDBusProxy::setAuthTypeEnabledForApp(KADAuthType authType,
                                                  KADAuthApplication authApp, bool enabled)
{
    auto reply = m_authProxy->SetAuthTypeEnabledForApp(authType, authApp, enabled);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "set auth type" << authType
                       << "auth application" << authApp
                       << "enabled " << enabled
                       << "failed," << reply.error().message();
    }
}

QList<KiranAuthDBusProxy::Device>
KiranAuthDBusProxy::getDevicesByType(KADAuthType type)
{
    QList<KiranAuthDBusProxy::Device> devices;
    auto reply = m_authProxy->GetDevicesForType(type);
    auto devicesJson = reply.value();

    QJsonDocument doc = QJsonDocument::fromJson(devicesJson.toUtf8());
    if (!doc.isArray())
    {
        return devices;
    }

    QJsonArray jsonArray = doc.array();
    for (int i = 0; i < jsonArray.count(); i++)
    {
        QJsonValue jsonValue = jsonArray.at(i);
        CONTINUE_IF_FALSE(jsonValue.isObject());

        QJsonObject object = jsonValue.toObject();
        devices << KiranAuthDBusProxy::Device{
            .id = object["deviceID"].toString(),
            .name = object["deviceName"].toString(),
            .path = object["objectPath"].toString()};
    }

    return devices;
}

QString KiranAuthDBusProxy::getDefaultDeviceID(KADAuthType type)
{
    auto pendingReply = m_authProxy->GetDefaultDeviceID(type);
    return pendingReply.value();
}

void KiranAuthDBusProxy::setDefaultDeviceID(KADAuthType type, const QString& deviceID)
{
    auto pendingReply = m_authProxy->SetDefaultDeviceID(type, deviceID);
    pendingReply.waitForFinished();
    if (pendingReply.isError())
    {
        KLOG_ERROR() << "set default device id" << type
                     << "device id" << deviceID
                     << "failed" << pendingReply.error().message();
    }
}

QList<KiranAuthDBusProxy::Driver>
KiranAuthDBusProxy::getDriversByType(KADAuthType type)
{
    QList<KiranAuthDBusProxy::Driver> drivers;

    auto pendingReply = m_authProxy->GetDriversForType(type);
    auto driversJson = pendingReply.value();

    QJsonDocument doc = QJsonDocument::fromJson(driversJson.toUtf8());
    if (!doc.isArray())
    {
        return drivers;
    }

    QJsonArray jsonArray = doc.array();
    for (int i = 0; i < jsonArray.count(); i++)
    {
        QJsonValue jsonValue = jsonArray.at(i);
        CONTINUE_IF_FALSE(jsonValue.isObject());

        QJsonObject object = jsonValue.toObject();
        drivers << KiranAuthDBusProxy::Driver{
            .name = object["driverName"].toString(),
            .enabled = object["enable"].toBool()};
    }

    return drivers;
}

void KiranAuthDBusProxy::setDriverEnalbe(const QString& driverName,bool enable)
{
    auto reply = m_authProxy->SetDrivereEnabled(driverName,enable);
    reply.waitForFinished();
}

bool KiranAuthDBusProxy::startEnroll(KADAuthType authType,
                                     const QString& featureName,
                                     const QString& extraInfo,
                                     QString& errorString)
{
    auto reply = m_authUserProxy->EnrollStart(authType, featureName, extraInfo);
    reply.waitForFinished();
    if (reply.isError())
    {
        errorString = reply.error().message();
        return false;
    }
    return true;
}

void KiranAuthDBusProxy::stopEnroll()
{
    auto reply = m_authUserProxy->EnrollStop();
    reply.waitForFinished();
}

QList<KiranAuthDBusProxy::Identification>
KiranAuthDBusProxy::getUserIdentifications(KADAuthType type)
{
    QList<KiranAuthDBusProxy::Identification> identifications;

    RETURN_VAL_IF_FALSE(m_authUserProxy, identifications);

    auto pendingReply = m_authUserProxy->GetIdentifications(type);
    auto identificationJson = pendingReply.value();

    QJsonDocument doc = QJsonDocument::fromJson(identificationJson.toUtf8());
    if (!doc.isArray())
    {
        return identifications;
    }

    QJsonArray jsonArray = doc.array();
    for (int i = 0; i < jsonArray.count(); i++)
    {
        QJsonValue jsonValue = jsonArray.at(i);
        CONTINUE_IF_FALSE(jsonValue.isObject());
        QJsonObject object = jsonValue.toObject();

        identifications << Identification{
            .name = object["name"].toString(),
            .IID = object["iid"].toString(),
            .dataID = object["data_id"].toString()};
    }

    return identifications;
}

void KiranAuthDBusProxy::deleteIdentification(const QString& iid)
{
    RETURN_IF_FALSE(m_authUserProxy);

    auto reply = m_authUserProxy->DeleteIdentification(iid);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "delete identification failed,"
                       << "identification id:" << iid;
    }
}

void KiranAuthDBusProxy::renameIdentification(const QString& iid, const QString& name)
{
    RETURN_IF_FALSE(m_authUserProxy);

    auto reply = m_authUserProxy->RenameIdentification(iid, name);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "rename identification failed,"
                       << "identification id:" << iid
                       << "name:" << name;
    }
}

void KiranAuthDBusProxy::init()
{
    m_authProxy = new KASAuthProxy(KASAuthProxy::staticInterfaceName(),
                                   "/com/kylinsec/Kiran/Authentication",
                                   QDBusConnection::systemBus());

    auto uid = getuid();
    auto reply = m_authProxy->FindUserByID(uid);
    auto userObject = reply.value();
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "find user by id" << uid << "failed," << reply.error().message();
        return;
    }

    m_authUserProxy = new KASAuthUserProxy(KASAuthProxy::staticInterfaceName(),
                                           userObject.path(),
                                           QDBusConnection::systemBus(),this);
    connect(m_authUserProxy, &KASAuthUserProxy::EnrollStatus, this, &KiranAuthDBusProxy::EnrollStatus);
}

KiranAuthDBusProxy::KiranAuthDBusProxy()
    : QObject()
{
}
