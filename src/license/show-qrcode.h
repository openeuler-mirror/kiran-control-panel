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
