#ifndef LICENSEINFOWIDGET_H
#define LICENSEINFOWIDGET_H

#include <QWidget>
#include "show-qrcode.h"
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

namespace Ui {
class LicenseInfoWidget;
}

class LicenseInfoWidget : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit LicenseInfoWidget(QString & mc_code, QString & lc_code);
    ~LicenseInfoWidget();

    void setMachineCode(QString & machine_code);
    void setLicenseCode(QString & license_code);

private:
     void iniUI();

private slots:
    void popupQRCode(QPoint oPoint,QObject *target);

signals:
    void sig_showQRCodeWgt(QPoint oPoint,QObject *target);

protected:
    bool eventFilter(QObject *target, QEvent *e);

private:
    Ui::LicenseInfoWidget *ui;
    ShowQRCode *showQRCode;
    QString machine_code;
    QString license_code;
};

#endif // LICENSEINFOWIDGET_H
