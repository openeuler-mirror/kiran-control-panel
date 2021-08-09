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

#ifndef SYSTEMINFORMATIONWIDGET_H
#define SYSTEMINFORMATIONWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include "license/active-guide-widget.h"
#include "license/license-info-widget.h"
#include "change-host-name-widget.h"
namespace Ui {
class SystemInformationWidget;
}

class LicenseAgreement;
class SystemInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemInformationWidget(QWidget *parent = 0);
    ~SystemInformationWidget();
    bool initUI();
    bool hasUnsavedOptions();

private:
    bool readSystemInfo(int infoType);
    void getJsonValueFromString(QString jsonString);
    bool readLicenseInfo();

private slots:
    void onBtnchangeHostName(void);
    void onBtnStatusClicked();
    void updateLicenseInfo(bool isregister);
    void updateHostName(bool isChanged,QString name);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent* painEvent);
    virtual QSize sizeHint () const ;

private:
    Ui::SystemInformationWidget *ui;

    bool isActive;
    QString mc_code;
    QString lc_code;
    // 授权状态
    quint16 license_status = 0;
    // 过期时间
    quint32 expired_time;
    // 安装时间
    quint32 start_time;
    QString installTime;

    ActGuideWidget *activeGuide;
    LicenseInfoWidget *licenseInfoWidget;
    ChangeHostNameWidget* hostNameWidget;
    LicenseAgreement *licenseAgreement;
};

#endif // SYSTEMINFORMATIONWIDGET_H
