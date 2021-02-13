/**
 * @file system-info-dbus.h
 * @brief 后端Dbus接口
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef SYSTEMINFODBUS_H
#define SYSTEMINFODBUS_H

#include <QString>

namespace InfoDbus {
    namespace SystemInfo {
        bool getSystemInfo(int infoType , QString &info);
        bool setHostName(QString name);
    }
    namespace KylinLicense {
        bool getLicenseJson(QString &licenseInfo);
        bool getMachineCode(QString& machineCode);
        bool registerByLicenseCode(QString licenseCode , QString &errorMsg);
        bool registerByUsbKey(QString &points, QString &errorMsg);
        bool registerOnline(QString ip ,QString &errorMsg);
        bool getServiceStatus(int &status);
    }
}


#endif // SYSTEMINFODBUS_H
