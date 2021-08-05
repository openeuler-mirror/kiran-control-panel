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
#include "qrcode-widget.h"
#include <QPaintEvent>
#include <QPainter>
#include <kiran-log/qt5-log-i.h>

QRCodeWidget::QRCodeWidget(QWidget* parent, const QString &text, int width, int height) :
    QLabel(parent),
    m_data(text)
{
    createQRcode(width,height);
}

QRCodeWidget::~QRCodeWidget()
{
    if(m_qrcode)
    {
        QRcode_free(m_qrcode);
    }
}

const QString &QRCodeWidget::getData()
{
    return m_data;
}

void QRCodeWidget::setData(const QString &data_)
{
    m_data = data_;
}

void QRCodeWidget::createQRcode(int width, int height)
{
    QPixmap QRPixmap;
    m_qrcode = QRcode_encodeString(m_data.toStdString().c_str(),2,QR_ECLEVEL_Q,QR_MODE_8,1);

    int qrcodeWidth = m_qrcode->width >0 ? m_qrcode->width : 1;
    double scaledWidth= (double)width / (double)qrcodeWidth;
    double scaledHeight =(double) height /(double) qrcodeWidth;
    QImage image = QImage(width, height, QImage::Format_ARGB32);

    QPainter painter(&image);
    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width, height);
    QColor foreground(Qt::black);
    painter.setBrush(foreground);

    for( qint32 y = 0; y < qrcodeWidth; y ++)
    {
       for(qint32 x = 0; x < qrcodeWidth; x++)
       {
           unsigned char b = m_qrcode->data[y * qrcodeWidth + x];
           if(b & 0x01)
           {
               QRectF r(x * scaledWidth, y * scaledHeight, scaledWidth, scaledHeight);
               painter.drawRects(&r, 1);
           }
       }
    }

    QRPixmap = QPixmap::fromImage(image);
    setPixmap(QRPixmap);
}
