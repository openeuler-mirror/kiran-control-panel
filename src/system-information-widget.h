#ifndef SYSTEMINFORMATIONWIDGET_H
#define SYSTEMINFORMATIONWIDGET_H

#include <QWidget>

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
    void changeCurrentHostName(void);

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
};

#endif // SYSTEMINFORMATIONWIDGET_H
