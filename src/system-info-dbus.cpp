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


bool InfoDbus::SystemInfo::getSystemInfo(int)
{

}

bool InfoDbus::SystemInfo::setHostName(QString)
{

}

bool InfoDbus::KylinLicense::getLicenseJson(QString &licenseInfo)
{

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
