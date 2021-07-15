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
    explicit LicenseInfoWidget(QString & mc_code, QString & lc_code,QWidget *parent=nullptr);
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
