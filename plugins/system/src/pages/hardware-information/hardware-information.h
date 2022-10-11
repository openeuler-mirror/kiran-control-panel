/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#ifndef HARDWAREINFORMATION_H
#define HARDWAREINFORMATION_H

#include <QImage>
#include <QStringList>
#include <QWidget>

namespace Ui
{
class HardwareInformation;
}

class HardwareInformation : public QWidget
{
    Q_OBJECT

public:
    explicit HardwareInformation(QWidget *parent = nullptr);
    ~HardwareInformation();

    void initUI();
private:
    bool parseHardwareInfoJson(const QString &json,
                               QString &memory,
                               QString &cpu,
                               QStringList &disk,
                               QStringList &graphics,
                               QStringList &eths);

private:
    Ui::HardwareInformation *ui;
    QStringList diskList;
    QStringList graphicsList;
    QStringList ethsList;
};

#endif  // HARDWAREINFORMATION_H
