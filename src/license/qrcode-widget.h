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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPaintEvent>
#include <QLabel>
#include <qrencode.h>

class QRCodeWidget : public QLabel
{
    Q_OBJECT

public:
    explicit QRCodeWidget(QWidget* parent,const QString &text,int width , int height);
    ~QRCodeWidget();

    const QString &getData();
    void setData(const QString &data_);

    void createQRcode(int width , int height);

private:
    QString m_data;
    QRcode *m_qrcode;
};

#endif // MAINWINDOW_H
