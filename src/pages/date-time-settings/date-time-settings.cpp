//
// Created by lxh on 2021/5/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_date-time-settings.h" resolved

#include "date-time-settings.h"
#include "timedate-interface.h"
#include "ui_date-time-settings.h"

#include <widget-property-helper.h>

DateTimeSettings::DateTimeSettings(QWidget *parent) : QWidget(parent), ui(new Ui::DateTimeSettings)
{
    ui->setupUi(this);
    init();
}

DateTimeSettings::~DateTimeSettings()
{
    delete ui;
}

void DateTimeSettings::init()
{
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save,Kiran::BUTTON_Default);
    connect(ui->btn_save, &QPushButton::clicked, this, &DateTimeSettings::handleButtonSaveClicked);
    connect(ui->btn_reset, &QPushButton::clicked, this, &DateTimeSettings::handleButtonResetClicked);
}

void DateTimeSettings::reset()
{
    ui->widget_setDate->reset();
    ui->widget_setTime->reset();
}

void DateTimeSettings::handleButtonSaveClicked()
{
    QDateTime dateTime;
    qint64 microsecondsSinceEpoch;
    dateTime.setDate(ui->widget_setDate->currentDate());
    dateTime.setTime(ui->widget_setTime->currentTime());
    microsecondsSinceEpoch = dateTime.toMSecsSinceEpoch() * 1000;
    QPair<bool, QString> res = ComKylinsecKiranSystemDaemonTimeDateInterface::instance()->SyncSetTime(microsecondsSinceEpoch, false);
}

void DateTimeSettings::handleButtonResetClicked()
{
    reset();
}
