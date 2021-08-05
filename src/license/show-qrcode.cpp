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
#include <QIcon>
#include "show-qrcode.h"
#include "ui_show-qrcode.h"

ShowQRCode::ShowQRCode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowQRCode)
{
    ui->setupUi(this);
    iniUI();
}

void ShowQRCode::iniUI()
{
    setWindowTitle(ShowQRCode::tr("QRcode of Machine and Activation Code"));
    setWindowIcon(QIcon(":/images/kylin-about.png"));
    this->setMaximumSize(230,230);
    this->setMinimumSize(230,230);
    ui->qrcode_layout->setMargin(10);
}


void ShowQRCode::setQRCode(const QString &text ,bool isMachineCode)
{
    qrcode = new QRCodeWidget(this,text,160,160);
    qrcode->setMinimumSize(160,160);
    qrcode->setMaximumSize(160,160);
    ui->qrcode_layout->addWidget(qrcode,Qt::AlignHCenter);
    ui->qrcode_layout->setAlignment(qrcode ,Qt::AlignHCenter );
    ui->qrcode_layout->setAlignment(qrcode ,Qt::AlignVCenter );

    if(!isMachineCode)
    {
        ui->label_qrcode_text->setText(tr("Scan QR code to get activation code"));
    }

}

ShowQRCode::~ShowQRCode()
{
    delete ui;
}
