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
