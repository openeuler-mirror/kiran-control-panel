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

// You may need to build the project (run Qt uic code generator) to get "ui_time-zone-settings.h" resolved

#include "time-zone-settings.h"
#include "ui_time-zone-settings.h"
#include <style-property.h>

using namespace Kiran;

TimezoneSettings::TimezoneSettings(QWidget *parent) : QWidget(parent), ui(new Ui::TimezoneSettings)
{
    ui->setupUi(this);
    init();
}

TimezoneSettings::~TimezoneSettings()
{
    delete ui;
}

void TimezoneSettings::init()
{
    StylePropertyHelper::setButtonType(ui->btn_save,BUTTON_Default);
    connect(ui->btn_save,&QPushButton::clicked,this,&TimezoneSettings::handleSaveButtonClicked);
    connect(ui->btn_reset,&QPushButton::clicked,this,&TimezoneSettings::handleResetButtonClicked);
}

void TimezoneSettings::reset()
{
    ui->timezone->reset();
}

void TimezoneSettings::handleSaveButtonClicked()
{
    ui->timezone->save();
}

void TimezoneSettings::handleResetButtonClicked()
{
    ui->timezone->reset();
}
