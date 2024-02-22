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
#pragma once
#include <QString>
#include <QList>
#include <QObject>
#include <kiran-authentication-service/kas-authentication-i.h>

class KASAuthProxy;
class KASAuthUserProxy;
class KiranAuthDBusProxy : public QObject
{
    Q_OBJECT
private:
    KiranAuthDBusProxy();

public:
    static KiranAuthDBusProxy *getInstance() { return m_instance; };
    static void globalInit();
    static void globalDeinit() { delete m_instance; };

    ~KiranAuthDBusProxy();

public:
    struct Device
    {
        QString id;
        QString name;
        QString path;
    };

    struct Driver
    {
        QString name;
        bool enabled;
    };

    struct Identification
    {
        QString name;
        QString IID;
        QString dataID;
    };

    bool getAuthTypeEnabled(KADAuthType type);
    void setAuthTypeEnabled(KADAuthType type,bool enabled);

    bool getAuthTypeEnabledForApp(KADAuthType authType,KADAuthApplication authApp);
    void setAuthTypeEnabledForApp(KADAuthType authType,KADAuthApplication authApp,bool enabled);

    QList<Device> getDevicesByType(KADAuthType type);

    QString getDefaultDeviceID(KADAuthType type);
    void setDefaultDeviceID(KADAuthType type,const QString& deviceID);

    QList<Driver> getDriversByType(KADAuthType type);
    void setDriverEnalbe(const QString& driverName,bool enable);

    bool startEnroll(KADAuthType type,const QString& featureName,const QString& extraInfo,QString& errorString);
    void stopEnroll();

    QList<Identification> getUserIdentifications(KADAuthType type);
    void deleteIdentification(const QString& iid);
    void renameIdentification(const QString& iid,const QString& name);

signals:
    void EnrollStatus(const QString& iid,bool isComplete,int progress,const QString& message);

private:
    void init();

private:
    static KiranAuthDBusProxy* m_instance;
    KASAuthProxy* m_authProxy = nullptr;
    KASAuthUserProxy* m_authUserProxy = nullptr;
};