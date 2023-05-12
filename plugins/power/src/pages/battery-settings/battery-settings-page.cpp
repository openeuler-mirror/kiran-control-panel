/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
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

#include "battery-settings-page.h"
#include "common/common.h"
#include "dbus/power.h"
#include "ui_battery-settings-page.h"

#include <kiran-session-daemon/power-i.h>
#include <kiran-switch-button.h>
#include <qt5-log-i.h>

using namespace Common;

BatterySettingsPage::BatterySettingsPage(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::BatterySettingsPage),
      m_powerInterface(PowerInterface::getInstance())
{
    ui->setupUi(this);
    init();
}

BatterySettingsPage::~BatterySettingsPage()
{
    delete ui;
}

QSize BatterySettingsPage::sizeHint() const
{
    return {500, 657};
}

void BatterySettingsPage::init()
{
    initUI();
    initConnection();
    load();
}

void BatterySettingsPage::initUI()
{
    // 计算机空闲时间设置
    static QList<int> idleTimeouts = {60, 180, 300, 600, 900, 1200, 1800, 2700, 3600, 7200, 10800, 14400, 18000, 0};
    foreach (const int& timeout, idleTimeouts)
    {
        ui->combo_idleTimeout->addItem(getTimeDescription(timeout), QVariant::fromValue(timeout));
        ui->combo_monitorOffTimeout->addItem(getTimeDescription(timeout), QVariant::fromValue(timeout));
    }

    // 计算机空闲进行的操作
    // clang-format off
    QMap<QString, int> computerIdleActionsMap = {
        {tr("Suspend"), POWER_ACTION_COMPUTER_SUSPEND},
        {tr("Shutdown"), POWER_ACTION_COMPUTER_SHUTDOWN},
        {tr("Hibernate"), POWER_ACTION_COMPUTER_HIBERNATE},
        {tr("Do nothing"), POWER_ACTION_NOTHING}
    };
    // clang-format on
    for (auto iter = computerIdleActionsMap.begin();
         iter != computerIdleActionsMap.end();
         iter++)
    {
        ui->combo_idleAction->addItem(iter.key(), QVariant::fromValue(iter.value()));
    }

    // 电池将要耗尽时进行的操作
    // clang-format off
    QMap<QString,int> powerRunOutActionsMap = {
        {tr("Suspend"), POWER_ACTION_COMPUTER_SUSPEND},
        {tr("Shutdown"), POWER_ACTION_COMPUTER_SHUTDOWN},
        {tr("Hibernate"), POWER_ACTION_COMPUTER_HIBERNATE},
        {tr("Do nothing"), POWER_ACTION_NOTHING}
    };
    // clang-format on
    for (auto iter = powerRunOutActionsMap.begin();
         iter != powerRunOutActionsMap.end();
         iter++)
    {
        ui->combo_noPowerAction->addItem(iter.key(), iter.value());
    }

    // 空闲时减少屏幕亮度
    m_switchIdleDimmed = new KiranSwitchButton(this);
    m_switchIdleDimmed->setAccessibleName("SwitchIdleReduceBrightness");
    ui->layout_reduceBrightness->insertWidget(ui->layout_reduceBrightness->count(), m_switchIdleDimmed);

    // 低电亮减少亮度
    m_switchChargeLowDimmed = new KiranSwitchButton(this);
    m_switchChargeLowDimmed->setAccessibleName("SwitchNoPowerReduceBrightness");
    ui->layout_lowPowerReduceBrightness->insertWidget(ui->layout_lowPowerReduceBrightness->count(), m_switchChargeLowDimmed);

    // 低电量节能模式
    m_chargeLowSaverSwitch = new KiranSwitchButton(this);
    m_chargeLowSaverSwitch->setAccessibleName("SwitchNoPowerReduceBrightness");
    ui->layout_lowPowerEco->insertWidget(ui->layout_lowPowerEco->count(), m_chargeLowSaverSwitch);
}

void BatterySettingsPage::initConnection()
{
    connect(ui->combo_idleTimeout, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::onIdleTimeOrActionChanged);
    connect(ui->combo_idleAction, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::onIdleTimeOrActionChanged);
    connect(ui->combo_noPowerAction, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::onBatteryChargeActionChanged);
    connect(ui->combo_monitorOffTimeout, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::onMonitorOffIdleTimeoutChanged);
    connect(m_switchIdleDimmed, &QAbstractButton::toggled,
            this, &BatterySettingsPage::onDisplayIdleDimmedToggled);
    connect(m_switchChargeLowDimmed, &QAbstractButton::toggled,
            this, &BatterySettingsPage::onChargeLowDimmedToggled);
    connect(m_chargeLowSaverSwitch, &QAbstractButton::toggled,
            this, &BatterySettingsPage::onChargeLowSaverToggled);
}

void BatterySettingsPage::load()
{
    auto setComboBoxCurrentData = [this](QComboBox* combo, const QVariant& var) -> bool
    {
        auto idx = combo->findData(var);
        if (idx == -1)
        {
            return false;
        }
        QSignalBlocker signalBlocker(combo);
        combo->setCurrentIndex(idx);
        return true;
    };

    // 空闲超过指定时间，执行指定操作
    auto computerIdleActionReply = m_powerInterface->GetIdleAction(POWER_DEVICE_TYPE_COMPUTER,
                                                                   POWER_SUPPLY_MODE_BATTERY);
    auto computerIdleAction = computerIdleActionReply.value();
    if (computerIdleActionReply.isError())
    {
        KLOG_WARNING() << "can't get computer idle timeout!,"
                       << computerIdleActionReply.error();
    }
    else
    {
        auto idleTimeout = computerIdleAction.idleTimeout;
        auto idleAction = computerIdleAction.idleAction;

        if (!setComboBoxCurrentData(ui->combo_idleTimeout, idleTimeout))
        {
            KLOG_WARNING("can't find computer idle timeout index(%d).", idleTimeout);
        }

        if (!setComboBoxCurrentData(ui->combo_idleAction, idleAction))
        {
            KLOG_WARNING("can't find computer idle action(%d).", idleAction);
        }
    }

    // 电量耗尽执行的操作
    auto batteryChargeActionReply = m_powerInterface->GetEventAction(POWER_EVENT_BATTERY_CHARGE_ACTION);
    auto batteryChargeAction = batteryChargeActionReply.value();
    if (batteryChargeActionReply.isError())
    {
        KLOG_WARNING() << "get lower power action failed," << batteryChargeActionReply.error();
    }
    else
    {
        if (!setComboBoxCurrentData(ui->combo_noPowerAction, batteryChargeAction))
        {
            KLOG_WARNING("can't find battery charge action(%d)", batteryChargeAction);
        }
    }

    // 空闲多久后，显示器将执行关闭
    auto monitorIdleTimeoutActionReply = m_powerInterface->GetIdleAction(POWER_DEVICE_TYPE_BACKLIGHT,
                                                                         POWER_SUPPLY_MODE_BATTERY);
    auto monitorIdleTimeoutAction = monitorIdleTimeoutActionReply.value();
    if (monitorIdleTimeoutActionReply.isError())
    {
        KLOG_WARNING() << "can't get monitor idle timeout!," << monitorIdleTimeoutActionReply.error();
    }
    else
    {
        auto idleTimeout = monitorIdleTimeoutAction.idleTimeout;
        if (!setComboBoxCurrentData(ui->combo_monitorOffTimeout, idleTimeout))
        {
            KLOG_WARNING("can't find monitor off timeout index(%d).", idleTimeout);
        }
    }

    // 空闲时减少亮度 
    QSignalBlocker idleDimmedEnabledBlocker(m_switchIdleDimmed);
    auto idleDimmedEnabled = m_powerInterface->displayIdleDimmedEnabled();
    m_switchIdleDimmed->setChecked(idleDimmedEnabled);

    // 低电量时减少亮度
    QSignalBlocker chargeLowDimmedBlocker(m_switchChargeLowDimmed);
    auto chargeLowDimmedEnabled = m_powerInterface->chargeLowDimmedEnabled();
    m_switchChargeLowDimmed->setChecked(chargeLowDimmedEnabled);

    // 低电量时自动开启节能模式
    QSignalBlocker chargeLowSaverBlocker(m_chargeLowSaverSwitch);
    auto chargeLowSaverEnabled =  m_powerInterface->chargeLowSaverEnabled();
    m_chargeLowSaverSwitch->setChecked(chargeLowSaverEnabled);
}

void BatterySettingsPage::onIdleTimeOrActionChanged(int idx)
{
    IdleAction idleAction{};
    if (!getCurrentIdleAction(idleAction))
    {
        return;
    }

    auto reply = m_powerInterface->SetIdleAction(POWER_DEVICE_TYPE_COMPUTER,
                                                 POWER_SUPPLY_MODE_BATTERY,
                                                 idleAction.idleTimeout,
                                                 idleAction.idleAction);
    if (reply.isError())
    {
        KLOG_WARNING() << "set idle action for computer failed" << reply.error();
    }
}

void BatterySettingsPage::onBatteryChargeActionChanged(int idx)
{
    int action = ui->combo_noPowerAction->currentData().toInt();
    auto reply = m_powerInterface->SetEventAction(POWER_EVENT_BATTERY_CHARGE_ACTION, action);

    reply.waitForFinished();

    if (reply.isError())
    {
        KLOG_WARNING() << "set no power action(" << action << ") failed," << reply.error();
    }
}

void BatterySettingsPage::onMonitorOffIdleTimeoutChanged(int idx)
{
    bool isOk = false;
    int displayOffTime = ui->combo_monitorOffTimeout->currentData().toInt(&isOk);
    auto reply = m_powerInterface->SetIdleAction(POWER_DEVICE_TYPE_BACKLIGHT,
                                                 POWER_SUPPLY_MODE_BATTERY,
                                                 displayOffTime,
                                                 POWER_ACTION_DISPLAY_OFF);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "set idle action for monitor failed," << reply.error();
    }
}

void BatterySettingsPage::onDisplayIdleDimmedToggled(bool checked)
{
    auto reply = m_powerInterface->EnableDisplayIdleDimmed(checked);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "BatterySetting EnableDisplayIdleDimmed" << checked << "failed," << reply.error();
    }
}

void BatterySettingsPage::onChargeLowDimmedToggled(bool checked)
{
    auto reply = m_powerInterface->EnableChargeLowDimmed(checked);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "BatterySetting EnableChargeLowDimmed" << checked << "failed," << reply.error();
    }
}

void BatterySettingsPage::onChargeLowSaverToggled(bool checked)
{
    auto reply = m_powerInterface->EnableChargeLowSaver(checked);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "BatterySetting EnableChargeLowSaver" << checked << "failed," << reply.error();
    }
}

bool BatterySettingsPage::getCurrentIdleAction(IdleAction& idleAction)
{
    bool toIntOk = false;

    int timeout = ui->combo_idleTimeout->currentData().toInt(&toIntOk);
    if (!toIntOk)
    {
        KLOG_WARNING() << "combobox " << ui->combo_idleTimeout->objectName()
                       << "item data can't convert to int," << ui->combo_idleTimeout->currentData();
        return false;
    }

    int action = (PowerAction)ui->combo_idleAction->currentData().toInt(&toIntOk);
    if (!toIntOk)
    {
        KLOG_WARNING() << "combobox " << ui->combo_idleAction->objectName()
                       << "imet data can't convert to int," << ui->combo_idleAction->currentData();
        return false;
    }

    idleAction.idleTimeout = timeout;
    idleAction.idleAction = action;

    return true;
}