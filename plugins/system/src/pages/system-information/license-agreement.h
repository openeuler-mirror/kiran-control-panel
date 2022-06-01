/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */
#ifndef LICENSEAGREEMENT_H
#define LICENSEAGREEMENT_H

#include <QWidget>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

namespace Ui
{
class LicenseAgreement;
}


class LicenseAgreement : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit LicenseAgreement(QWidget *parent, Qt::WindowFlags windowFlags=Qt::Window);
    ~LicenseAgreement();
    QString getEulaText();
    void setEULA();
    void setVersionLicnese();
    //void setLicenseType(int type);

public slots:
    void exportLicense();

private:
    QString getLocaleLang();

private:
    Ui::LicenseAgreement *ui;
    int m_licenseType;
};

#endif  // LICENSEAGREEMENT_H
