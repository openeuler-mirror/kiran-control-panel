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
#include <kiran-log/qt5-log-i.h>

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
        KLOG_WARNING() <<"Faile to render qrcode: %s\n",
                  zint.lastError().toStdString().c_str();
    }
}
