#include "qrcode-widget.h"
#include <QPaintEvent>
#include <QDebug>

QRCodeWidget::QRCodeWidget(QWidget* parent,const QString &text) :
    QWidget(parent),
    data(text)
{
    zint.setSymbol(BARCODE_QRCODE);
    zint.setBorderWidth(2);
}

const QString &QRCodeWidget::getData()
{
    return data;
}

void QRCodeWidget::setData(const QString &data_)
{
    data = data_;
}

void QRCodeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QWidget::paintEvent(event);

    zint.setText(data);
    zint.render(painter, event->rect());
    if (zint.hasErrors()) {
        qCritical("Faile to render qrcode: %s\n",
                  zint.lastError().toStdString().c_str());
    }
}
