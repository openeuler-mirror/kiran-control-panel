/**
 * @file system-info-dbus.cpp
 * @brief 后端Dbus接口
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "system-info-dbus.h"
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusArgument>
#include <QDBusReply>
#include <QDBusSignature>
#include <QDebug>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusReply>
#include <QObject>
#include <iostream>
#include <kiran-cc-daemon/kiran-system-daemon/systeminfo_i.h>
#include <kylin-license/license_i.h>

#define SYSTEMINFO_DBUS_INTERFACE           "com.kylinsec.Kiran.SystemDaemon.SystemInfo"
#define METHOD_GET_SYSTEMINFO               "GetSystemInfo"
#define METHOD_SET_HOSTNAME                 "SetHostName"

#define KIRAN_LICENSE_INTERFACE             "com.kylinsec.Kiran.LicenseHelper"
#define METHOD_GET_LICENSE                  "GetLicense"
#define METHOD_GET_LICENSE_JSON             "GetLicenseJson"
#define METHOD_GET_MACHINE_CODE             "GetMachineCode"
#define METHOD_REGISTER_BY_LICENSE_CODE     "RegisterByLicenseCode"
#define METHOD_REGISTER_BY_USEBKEY          "RegisterByUsbKey"
#define METHOD_REGISTER_ONLINE              "RegisterOnLine"
#define METHOD_GETSERVICESTATUS             "GetServiceStatus"

#define TIMEOUT_MS 5000
#define TIMEOUT_MS_ONLINE 10000


bool InfoDbus::SystemInfo::getSystemInfo(int infoType , QString &info)
{
    QDBusMessage msgMethodCall = QDBusMessage::createMethodCall(SYSTEMINFO_DBUS_NAME,
                                                                SYSTEMINFO_OBJECT_PATH,
                                                                SYSTEMINFO_DBUS_INTERFACE,
                                                                METHOD_GET_SYSTEMINFO);

    msgMethodCall << infoType;

    QDBusMessage msgReply = QDBusConnection::systemBus().call(msgMethodCall,
                                                              QDBus::Block,
                                                              TIMEOUT_MS);


    qDebug() << "msgReply " << msgReply <<endl;
    QString errorMsg;
    if(msgReply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = msgReply.arguments();
        if( args.size() < 1 ){
            errorMsg = "arguments size < 1";
            goto failed;
        }
        QVariant firstArg = args.takeFirst();
        info = firstArg.toString();
        return true;
    }
    else if(msgReply.type() == QDBusMessage::ErrorMessage)
    {
            errorMsg = "";
            goto failed;
    }
failed:
    qWarning() << SYSTEMINFO_DBUS_NAME << METHOD_GET_SYSTEMINFO
               << msgReply.errorName() << msgReply.errorMessage() << errorMsg;
    return false;
}

bool InfoDbus::SystemInfo::setHostName(QString name)
{
    QDBusMessage msgMethodCall = QDBusMessage::createMethodCall(SYSTEMINFO_DBUS_NAME,
                                                                SYSTEMINFO_OBJECT_PATH,
                                                                SYSTEMINFO_DBUS_INTERFACE,
                                                                METHOD_SET_HOSTNAME);

    msgMethodCall << name;

    QDBusMessage msgReply = QDBusConnection::systemBus().call(msgMethodCall,
                                                              QDBus::Block,
                                                              TIMEOUT_MS);


    qDebug() << "msgReply " << msgReply <<endl;
    QString errorMsg;
    if(msgReply.type() == QDBusMessage::ReplyMessage)
    {
        return true;
    }
    else if(msgReply.type() == QDBusMessage::ErrorMessage)
    {
            errorMsg = "";
            goto failed;
    }

failed:
    qWarning() << SYSTEMINFO_DBUS_NAME << METHOD_SET_HOSTNAME
               << msgReply.errorName() << msgReply.errorMessage() << errorMsg;
    return false;
}

bool InfoDbus::KylinLicense::getLicenseJson(QString &licenseInfo)
{
    QDBusMessage msgMethodCall = QDBusMessage::createMethodCall(LICENSE_DBUS_NAME,
                                                                LICENSE_DBUS_OBJECT_PATH,
                                                                KIRAN_LICENSE_INTERFACE,
                                                                METHOD_GET_LICENSE_JSON);
    QDBusMessage msgReply = QDBusConnection::systemBus().call(msgMethodCall,
                                                              QDBus::Block,
                                                              TIMEOUT_MS);
    qDebug() << "msgReply " << msgReply <<endl;
    QString errorMsg;
    if(msgReply.type() == QDBusMessage::ReplyMessage){
        QList<QVariant> args = msgReply.arguments();
        if( args.size() < 1 ){
            errorMsg = "arguments size < 1";
            goto failed;
        }
        QVariant firstArg = args.takeFirst();
        licenseInfo = firstArg.toString();
        return true;
    }

failed:
    qWarning() << LICENSE_DBUS_NAME << METHOD_GET_LICENSE_JSON
               << msgReply.errorName() << msgReply.errorMessage() << errorMsg;
    return false;
}

bool InfoDbus::KylinLicense::getMachineCode(QString &machineCode)
{

}

bool InfoDbus::KylinLicense::registerByLicenseCode(QString licenseCode, QString &errorMsg)
{

}

bool InfoDbus::KylinLicense::registerByUsbKey(QString &points, QString &errorMsg)
{

}

bool InfoDbus::KylinLicense::registerOnline(QString ip, QString &errorMsg)
{

}

bool InfoDbus::KylinLicense::getServiceStatus(int &status)
{

}
