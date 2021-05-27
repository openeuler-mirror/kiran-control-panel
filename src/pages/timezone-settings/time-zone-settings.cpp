//
// Created by lxh on 2021/5/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_time-zone-settings.h" resolved

#include "time-zone-settings.h"
#include "ui_time-zone-settings.h"
#include <widget-property-helper.h>

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
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save,Kiran::BUTTON_Default);
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
