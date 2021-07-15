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

#ifndef HARDWAREINFORMATIONWIDGET_H
#define HARDWAREINFORMATIONWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QImage>

namespace Ui {
class HardwareInformationWidget;
}

class HardwareInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HardwareInformationWidget(QWidget *parent = 0);
    ~HardwareInformationWidget();
    bool initUI(void);

private:
    bool readHardwareInfo(int infoType);
    void getJsonValueFromString(QString jsonString);
    void showListInfo();
    void setCpuLogo(QString cpuModel);
    void scaledPixmap(QImage img);

private:
    Ui::HardwareInformationWidget *ui;
    QStringList diskList;
    QStringList graphicsList;
    QStringList ethsList;
};

#endif // HARDWAREINFORMATIONWIDGET_H
