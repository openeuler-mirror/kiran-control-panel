/**
 * @file system-information-widget.h
 * @brief  获取系统信息，包括授权信息，并显示在界面中，提供用户授权的接口
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
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

class SystemInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemInformationWidget(QWidget *parent = 0);
    ~SystemInformationWidget();

private:
    void initUI();
    void readSystemInfo(int infoType);
    void getJsonValueFromString(QString jsonString);
    void readLicenseInfo();

private slots:
    void onBtnchangeHostName(void);
    void onBtnStatusClicked();
    void updateLicenseInfo(bool isregister);
    void updateHostName(bool isChanged,QString name);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent* painEvent);

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
};

#endif // SYSTEMINFORMATIONWIDGET_H
