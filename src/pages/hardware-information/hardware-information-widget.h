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
