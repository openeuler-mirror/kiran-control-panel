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
#include "power-profiles-wrapper.h"
#include <qt5-log-i.h>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QVariantList>
#include "power_profiles_proxy.h"

#define POWERPROFILES_SERVICE_NAME "net.hadess.PowerProfiles"
#define POWERPROFILES_SERVICE_PATH "/net/hadess/PowerProfiles"
#define POWERPROFILES_SERVICE_PROPERTY_ACTIVE_PROFILE "ActiveProfile"
#define POWERPROFILES_SERVICE_PROPERTY_PROFILES "Profiles"

#define FREEDESKTOP_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define FREEDESKTOP_PROPERTIES_METHOD_GET "Get"
#define FREEDESKTOP_PROPERTIES_METHOD_SET "Set"
#define FREEDESKTOP_PROPERTIES_SIGNAL_PROPERTIES_CHANGED "PropertiesChanged"

Q_DECLARE_METATYPE(QList<QVariantMap>)
PowerProfilesWrapper::PowerProfilesWrapper(QObject* parent)
    : QObject(parent),
      m_profileProxy(new PowerProfilesProxy(POWERPROFILES_SERVICE_NAME, POWERPROFILES_SERVICE_PATH, QDBusConnection::systemBus(), this))
{
    qDBusRegisterMetaType<QList<QVariantMap>>();
    if (QDBusConnection::systemBus().interface()->isServiceRegistered(POWERPROFILES_SERVICE_NAME))
    {
        m_isValid = true;
        auto powerProfiles = m_profileProxy->profiles();
        for (auto powerProfile : powerProfiles)
        {
            KLOG_DEBUG() << powerProfile["Profile"].toString()<< powerProfile["Driver"].toString();
            if (powerProfile.contains("Profile"))
            {
                m_profiles << profileNameTransToLocale(powerProfile["Profile"].toString());
            }
        }

        auto activeProfile = m_profileProxy->activeProfile();
        m_activeProfile = activeProfile;
        connect(m_profileProxy, &PowerProfilesProxy::ActiveProfileChanged,
                this, &PowerProfilesWrapper::onActiveProfileChanged);
    }
    else
    {
        KLOG_WARNING() << "power profiles init failed," << POWERPROFILES_SERVICE_NAME << "isn't registered";
    }
}

PowerProfilesWrapper::~PowerProfilesWrapper()
{
}

bool PowerProfilesWrapper::isValid()
{
    return m_isValid;
}

QStringList PowerProfilesWrapper::supportedProfiles()
{
    return m_profiles;
}

QString PowerProfilesWrapper::activeProfile()
{
    return profileNameTransToLocale(m_activeProfile);
}

void PowerProfilesWrapper::setActiveProfile(const QString& profileName)
{
    QString value = localeTransToProfileName(profileName);
    m_profileProxy->setActiveProfile(value);
}

void PowerProfilesWrapper::onActiveProfileChanged(const QString& profileName)
{
    m_activeProfile = profileName;
    QString localeProfileName = profileNameTransToLocale(profileName);
    emit activeProfileChanged(localeProfileName);
}

QString PowerProfilesWrapper::profileNameTransToLocale(const QString& profileName)
{
    QMap<QString, QString> transMap = {
        {"power-saver", tr("power-saver")},
        {"balanced", tr("balanced")},
        {"performance", tr("performance")}};

    QString localeProfileName = profileName;
    if (transMap.contains(profileName))
    {
        localeProfileName = transMap[profileName];
    }

    return localeProfileName;
}

QString PowerProfilesWrapper::localeTransToProfileName(const QString& locale)
{
    QMap<QString, QString> transMap = {
        {tr("power-saver"), "power-saver"},
        {tr("balanced"), "balanced"},
        {tr("performance"), "performance"}};

    QString profileName = locale;
    if (transMap.contains(locale))
    {
        profileName = transMap[locale];
    }

    return profileName;
}
