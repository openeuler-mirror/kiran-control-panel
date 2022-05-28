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
#include "show-qrcode.h"
#include <QIcon>
#include "ui_show-qrcode.h"

ShowQRCode::ShowQRCode(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::ShowQRCode)
{
    ui->setupUi(this);
    iniUI();
}

void ShowQRCode::iniUI()
{
    setWindowFlags(Qt::X11BypassWindowManagerHint);
    setWindowTitle(ShowQRCode::tr("QRcode of Machine and Activation Code"));
    setWindowIcon(QIcon(":/images/kylin-about.png"));
    setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setMaximumSize(230, 230);
    this->setMinimumSize(230, 230);
    ui->qrcode_layout->setMargin(10);
}

void ShowQRCode::setQRCode(const QString &text, bool isMachineCode)
{
    qrcode = new QRCodeWidget(this, text, 160, 160);
    qrcode->setMinimumSize(160, 160);
    qrcode->setMaximumSize(160, 160);
    ui->qrcode_layout->addWidget(qrcode, Qt::AlignHCenter);
    ui->qrcode_layout->setAlignment(qrcode, Qt::AlignHCenter);
    ui->qrcode_layout->setAlignment(qrcode, Qt::AlignVCenter);

    if (!isMachineCode)
    {
        ui->label_qrcode_text->setText(tr("Scan QR code to get activation code"));
    }
}

ShowQRCode::~ShowQRCode()
{
    delete ui;
}
