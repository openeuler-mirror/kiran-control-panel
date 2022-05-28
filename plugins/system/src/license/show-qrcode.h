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
#ifndef SHOWQRCODE_H
#define SHOWQRCODE_H

#include <QWidget>
#include "qrcode-widget.h"

namespace Ui {
class ShowQRCode;
}

class ShowQRCode : public QWidget
{
    Q_OBJECT

public:
    explicit ShowQRCode(QWidget *parent = 0);
    ~ShowQRCode();

private:
    void iniUI();

public:
    void setQRCode(const QString &text ,bool isMachineCode);

private:
    Ui::ShowQRCode *ui;
    QRCodeWidget * qrcode;


};

#endif // SHOWQRCODE_H
