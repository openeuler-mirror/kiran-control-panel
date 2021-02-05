#ifndef SYSTEMINFODBUS_H
#define SYSTEMINFODBUS_H

#include <QString>

namespace InfoDbus {
    namespace SystemInfo {
        bool getSystemInfo(int);
        bool setHostName(QString);
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
