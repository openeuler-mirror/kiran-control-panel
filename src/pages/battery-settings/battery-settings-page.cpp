//
// Created by lxh on 2021/4/6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_battery-settings-page.h" resolved

#include "battery-settings-page.h"
#include "ui_battery-settings-page.h"

BatterySettingsPage::BatterySettingsPage(QWidget *parent) : QWidget(parent), ui(new Ui::BatterySettingsPage)
{
    ui->setupUi(this);
    init();
}

BatterySettingsPage::~BatterySettingsPage()
{
    delete ui;
}

void BatterySettingsPage::init()
{
    initUI();
    initConnection();
    load();
}

void BatterySettingsPage::initUI()
{

}

void BatterySettingsPage::initConnection()
{

}

void BatterySettingsPage::load()
{

}
