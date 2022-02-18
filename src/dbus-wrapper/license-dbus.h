#ifndef LICENSEDBUS_H
#define LICENSEDBUS_H

#include <QString>

namespace InfoDbus
{
namespace KylinLicense
{
bool getLicenseJson(QString &licenseInfo);
bool getMachineCode(QString &machineCode);
bool registerByLicenseCode(QString licenseCode, QString &errorMsg);
bool registerByUsbKey(QString &points, QString &errorMsg);
bool registerOnline(QString ip, QString &errorMsg);
bool getServiceStatus(int &status);
}  // namespace KylinLicense
}  // namespace InfoDbus

#endif  // LICENSEDBUS_H
