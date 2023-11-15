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
#include "kiran-date-picker-widget.h"
#include "ui_kiran-date-picker-widget.h"

KiranDatePickerWidget::KiranDatePickerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranDatePickerWidget)
{
    ui->setupUi(this);
    initUI();
    setAccessibleName("DatePicker");
}

KiranDatePickerWidget::~KiranDatePickerWidget()
{
    delete ui;
}

QDate KiranDatePickerWidget::currentDate()
{
    QDate date;
    date.setDate(ui->spinbox_year->value(),
                 ui->spinbox_month->value(),
                 ui->spinbox_day->value());

    return date;
}

void KiranDatePickerWidget::setCurrentDate(QDate date)
{
    ui->spinbox_year->setValue(date.year());
    ui->spinbox_month->setValue(date.month());
    ui->spinbox_day->setValue(date.day());
}

void KiranDatePickerWidget::reset()
{
    setCurrentDate(QDate::currentDate());
}

void KiranDatePickerWidget::initUI()
{
    //年
    ui->spinbox_year->setMaximum(2070);
    ui->spinbox_year->setMinimum(1970);
    ui->spinbox_year->setValue(1970);
    connect(ui->spinbox_year,QOverload<int>::of(&QSpinBox::valueChanged),[this](int year){
        //年份切换只影响二月天数
        if(ui->spinbox_month->value()!=2){
            return;
        }
        QDate date(year,ui->spinbox_month->value(),1);
        ui->spinbox_day->setMaximum(date.daysInMonth());
    });

    //月
    ui->spinbox_month->setMaximum(12);
    ui->spinbox_month->setMinimum(1);
    ui->spinbox_month->setValue(1);
    connect(ui->spinbox_month,QOverload<int>::of(&QSpinBox::valueChanged),[this](int month){
        QDate date(ui->spinbox_year->value(),month,1);
        ui->spinbox_day->setMaximum(date.daysInMonth());
    });

    //日
    ui->spinbox_day->setMinimum(1);
    QDate date(ui->spinbox_year->value(),ui->spinbox_month->value(),1);
    ui->spinbox_day->setMaximum(date.daysInMonth());
    setCurrentDate(QDate::currentDate());
}
