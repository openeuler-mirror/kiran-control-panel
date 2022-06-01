/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-cpanel-power is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "cpanel-power-widget.h"
#include "pages/battery-settings/battery-settings-page.h"
#include "pages/general-settings/general-settings-page.h"
#include "pages/power-settings/power-settings-page.h"
#include "kiran-session-daemon/power-i.h"
#include "dbus/power.h"
#include "ui_cpanel-power-widget.h"

#include <QDebug>
#include <QFile>
#include <QIcon>

enum KiranPowerPages
{
    PAGE_GENERAL_SETTINGS,
    PAGE_POWER_SETTINGS,
    PAGE_BATTERY_SETTINGS
};

CPanelPowerWidget::CPanelPowerWidget(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::CPanelPowerWidget),
      m_powerInterface(PowerInterface::instance())
{
    ui->setupUi(this);
    init();
}

CPanelPowerWidget::~CPanelPowerWidget()
{
    delete ui;
}

void CPanelPowerWidget::init()
{
    initUI();
    initConnection();
    loadStyleSheet();
    ui->sidbar->setCurrentRow(PAGE_GENERAL_SETTINGS);
}

void CPanelPowerWidget::initUI()
{
    QListWidgetItem* siderBarItem = nullptr;

    ui->sidbar->setIconSize(QSize(16, 16));

    ///通用设置
    QIcon generalSettingsIcon;
    generalSettingsIcon.addFile(":/images/kcp-power-general-settings.svg", QSize(16, 16), QIcon::Normal);
    siderBarItem = new QListWidgetItem(generalSettingsIcon, tr("General Settings"));
    siderBarItem->setData(Qt::UserRole, PAGE_GENERAL_SETTINGS);
    ui->sidbar->insertItem(PAGE_GENERAL_SETTINGS, siderBarItem);

    m_generalSettings = new GeneralSettingsPage(this);
    ui->stackedWidget->insertWidget(PAGE_GENERAL_SETTINGS, m_generalSettings);

    ///电源设置
    QIcon powerSettingsIcon;
    powerSettingsIcon.addPixmap(QPixmap(":/images/kcp-power-power-settings.svg"), QIcon::Normal);
    powerSettingsIcon.addPixmap(QPixmap(":/images/power-settings-active.svg"), QIcon::Active);
    siderBarItem = new QListWidgetItem(powerSettingsIcon, tr("Power Settings"));
    siderBarItem->setData(Qt::UserRole, PAGE_POWER_SETTINGS);
    ui->sidbar->insertItem(PAGE_POWER_SETTINGS, siderBarItem);

    m_powerSettings = new PowerSettingsPage(this);
    ui->stackedWidget->insertWidget(PAGE_POWER_SETTINGS, m_powerSettings);

#ifndef TEST
    if(m_powerInterface->on_battery())
#endif
    {
        ///电池设置
        QIcon batterySettingsIcon(":/images/kcp-power-battery-settings.svg");
        siderBarItem = new QListWidgetItem(batterySettingsIcon, tr("Battery Settings"), ui->sidbar);
        siderBarItem->setData(Qt::UserRole, PAGE_BATTERY_SETTINGS);
        ui->sidbar->insertItem(PAGE_BATTERY_SETTINGS, siderBarItem);
    }

    m_batterySettings = new BatterySettingsPage(this);
    ui->stackedWidget->insertWidget(PAGE_BATTERY_SETTINGS, m_batterySettings);
}

void CPanelPowerWidget::initConnection()
{
    connect(ui->sidbar, &QListWidget::currentItemChanged, [this](QListWidgetItem* current, QListWidgetItem* pre) {
        ui->stackedWidget->setCurrentIndex(current->data(Qt::UserRole).toInt());
    });
}

void CPanelPowerWidget::loadStyleSheet()
{
    QFile file(":/themes/black.qss");
    if (file.open(QIODevice::ReadOnly))
    {
        this->setStyleSheet(file.readAll() + qApp->styleSheet());
    }
    else
    {
        qWarning() << "load style sheet failed";
    }
}