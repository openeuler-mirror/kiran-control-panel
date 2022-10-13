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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#ifndef KIRANDATEPICKERWIDGET_H
#define KIRANDATEPICKERWIDGET_H

#include <QWidget>
#include <QDate>

namespace Ui {
class KiranDatePickerWidget;
}

class KiranDatePickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranDatePickerWidget(QWidget *parent = nullptr);
    ~KiranDatePickerWidget();

    QDate currentDate();
    void setCurrentDate(QDate date);

    void reset();
private:
    void initUI();
private:
    Ui::KiranDatePickerWidget *ui;
};

#endif // KIRANDATEPICKERWIDGET_H
