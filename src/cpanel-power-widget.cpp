//
// Created by lxh on 2021/4/6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_kiran-power-manager-widget.h" resolved

#include "cpanel-power-widget.h"
#include "battery-settings-page.h"
#include "general-settings-page.h"
#include "kiran-session-daemon/power_i.h"
#include "power-settings-page.h"
#include "power.h"
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
    generalSettingsIcon.addFile(":/images/general-settings.svg", QSize(16, 16), QIcon::Normal);
    siderBarItem = new QListWidgetItem(generalSettingsIcon, tr("General Settings"));
    siderBarItem->setData(Qt::UserRole, PAGE_GENERAL_SETTINGS);
    ui->sidbar->insertItem(PAGE_GENERAL_SETTINGS, siderBarItem);

    m_generalSettings = new GeneralSettingsPage(this);
    ui->stackedWidget->insertWidget(PAGE_GENERAL_SETTINGS, m_generalSettings);

    ///电源设置
    QIcon powerSettingsIcon;
    powerSettingsIcon.addPixmap(QPixmap(":/images/power-settings.svg"), QIcon::Normal);
    powerSettingsIcon.addPixmap(QPixmap(":/images/power-settings-active.svg"), QIcon::Active);
    siderBarItem = new QListWidgetItem(powerSettingsIcon, tr("Power Settings"));
    siderBarItem->setData(Qt::UserRole, PAGE_POWER_SETTINGS);
    ui->sidbar->insertItem(PAGE_POWER_SETTINGS, siderBarItem);

    m_powerSettings = new PowerSettingsPage(this);
    ui->stackedWidget->insertWidget(PAGE_POWER_SETTINGS, m_powerSettings);

    ///NOTE:暂时屏蔽做开发
    //    if(m_powerInterface->on_battery())
    {
        ///电池设置
        QIcon batterySettingsIcon;
        batterySettingsIcon.addPixmap(QPixmap(":/images/battery-settings.svg"), QIcon::Normal);
        batterySettingsIcon.addPixmap(QPixmap(":/images/battery-settings-active.svg"), QIcon::Active);
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