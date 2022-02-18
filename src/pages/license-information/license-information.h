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

private:
    bool readLicenseInfo();
    void getJsonValueFromString(QString jsonString);

public slots:
    void onBtnStatusClicked();
    void updateLicenseInfo(bool isregister);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *painEvent);
    virtual QSize sizeHint() const;

private:
    Ui::LicenseInformation *ui;
    bool m_isActive;
    QString m_mcCode;
    QString m_lcCode;
    // 授权状态
    quint16 m_licenseStatus = 0;
    // 过期时间
    qlonglong m_expiredTime;
    // 安装时间
    qlonglong m_startTime;
    QString m_installTime;

    ActGuideWidget *m_activeGuide;
    LicenseInfoWidget *m_licenseInfoWidget;
};

#endif  // LICENSEINFORMATION_H
