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
    qrcode = new QRCodeWidget(this,text);
    qrcode->setMinimumSize(160,160);
    qrcode->setMaximumSize(160,160);
    ui->qrcode_layout->addWidget(qrcode,Qt::AlignHCenter);
    ui->qrcode_layout->setAlignment(qrcode ,Qt::AlignHCenter );
    ui->qrcode_layout->setAlignment(qrcode ,Qt::AlignVCenter );

    if(!isMachineCode)
    {
        ui->label_qrcode_text->setText(tr("Scan QR code to get Activation code"));
    }

}

ShowQRCode::~ShowQRCode()
{
    delete ui;
}
