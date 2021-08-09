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
