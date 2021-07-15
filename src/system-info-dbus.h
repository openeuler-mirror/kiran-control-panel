/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
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
