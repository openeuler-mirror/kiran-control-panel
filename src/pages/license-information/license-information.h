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

#ifndef LICENSEINFORMATION_H
#define LICENSEINFORMATION_H

#include <QWidget>

#include "license/active-guide-widget.h"
#include "license/license-info-widget.h"
namespace Ui
{
class LicenseInformation;
}

class LicenseInformation : public QWidget
{
    Q_OBJECT

public:
    explicit LicenseInformation(QWidget *parent = 0);
    ~LicenseInformation();
    virtual QSize sizeHint() const;

private:
    bool readLicenseInfo();
    void getJsonValueFromString(QString jsonString);

public slots:
    void onBtnStatusClicked();
    void updateLicenseInfo(bool isregister);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *);

private:
    Ui::LicenseInformation *ui;
    bool m_isActive;
    QString m_mcCode;
    QString m_lcCode;
    // 授权状态
    quint16 m_licenseStatus;
    // 过期时间
    qlonglong m_expiredTime;
    // 安装时间
    qlonglong m_startTime;
    QString m_installTime;

    ActGuideWidget *m_activeGuide;
    LicenseInfoWidget *m_licenseInfoWidget;
};

#endif  // LICENSEINFORMATION_H
