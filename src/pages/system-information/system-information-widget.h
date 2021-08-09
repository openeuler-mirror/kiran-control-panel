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
