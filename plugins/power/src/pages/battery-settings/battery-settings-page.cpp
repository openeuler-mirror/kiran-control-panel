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


#include "battery-settings-page.h"
#include "common/common.h"
#include "dbus/power.h"
#include "ui_battery-settings-page.h"

#include <qt5-log-i.h>
#include <kiran-session-daemon/power-i.h>
#include <kiran-switch-button.h>

#define LOW_POWER_REDUCE_BRIGHTNESS_SCALE 50

using namespace Common;

BatterySettingsPage::BatterySettingsPage(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::BatterySettingsPage),
      m_powerInterface(PowerInterface::instance())
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
    return {500,657};
}

void BatterySettingsPage::init()
{
    initUI();
    initConnection();
    load();
}

void BatterySettingsPage::initUI()
{
    ///计算机空闲时间设置
    static QList<int> idleTimeouts = {60, 180, 300, 600, 900, 1200, 1800, 2700, 3600, 7200, 10800, 14400, 18000, 0};
    foreach (const int& timeout, idleTimeouts)
    {
        ui->combo_idleTimeout->addItem(getTimeDescription(timeout), QVariant::fromValue(timeout));
        ui->combo_monitorOffTimeout->addItem(getTimeDescription(timeout), QVariant::fromValue(timeout));
    }

    ///计算机空闲进行的操作
    // clang-format off
    QMap<QString, int> computerIdleActionsMap = {
        {tr("Display Off"), POWER_ACTION_DISPLAY_OFF},
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

    ///电池将要耗尽时进行的操作
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

    ///空闲时减少屏幕亮度
    m_reduceBrightnessSwitch = new KiranSwitchButton(this);
    ui->layout_reduceBrightness->insertWidget(ui->layout_reduceBrightness->count(), m_reduceBrightnessSwitch);
}

void BatterySettingsPage::initConnection()
{
    connect(ui->combo_idleTimeout, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::handleIdleTimeActionCurrentIdxChanged);
    connect(ui->combo_idleAction, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::handleIdleTimeActionCurrentIdxChanged);
    connect(ui->combo_noPowerAction, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::handlePowerRunOutActionCurrentIdxChanged);
    connect(ui->combo_monitorOffTimeout, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BatterySettingsPage::handleMonitorOffTimeoutCurrentIdxChanged);
    connect(m_reduceBrightnessSwitch, &QAbstractButton::toggled,
            this, &BatterySettingsPage::handleSwitchReduceBrightnessToggled);
}

void BatterySettingsPage::load()
{
    int findComboIdx;

    ///加载计算机空闲超时时间和操作
    auto getComputerIdleTimeoutReply = m_powerInterface->GetIdleAction(POWER_DEVICE_TYPE_COMPUTER,
                                                                       POWER_SUPPLY_MODE_BATTERY);
    getComputerIdleTimeoutReply.waitForFinished();
    if (getComputerIdleTimeoutReply.isError())
    {
        KLOG_WARNING() << "can't get computer idle timeout!," << getComputerIdleTimeoutReply.error();
    }
    else
    {
        IdleAction action = getComputerIdleTimeoutReply.value();
        findComboIdx = ui->combo_idleTimeout->findData(action.idleTimeout);
        if (findComboIdx == -1)
        {
            KLOG_WARNING("can't find computer timeout(%d),append it to combobox!", action.idleTimeout);
            ui->combo_idleTimeout->addItem(getTimeDescription(action.idleTimeout), action.idleTimeout);
            findComboIdx = ui->combo_idleTimeout->findData(action.idleTimeout);
        }
        ui->combo_idleTimeout->blockSignals(true);
        ui->combo_idleTimeout->setCurrentIndex(findComboIdx);
        ui->combo_idleTimeout->blockSignals(false);

        findComboIdx = ui->combo_idleAction->findData(action.idleAction);
        if (findComboIdx == -1)
        {
            KLOG_WARNING("can't find computer idle action(%d),append it to combobox!", action.idleAction);
            ui->combo_idleAction->addItem(QString::number(action.idleAction), action.idleAction);
            findComboIdx = ui->combo_idleAction->findData(action.idleAction);
        }
        ui->combo_idleAction->blockSignals(true);
        ui->combo_idleAction->setCurrentIndex(findComboIdx);
        ui->combo_idleAction->blockSignals(false);
    }

    ///加载显示器空闲熄灭时间
    auto getMonitorIdleTimeoutReply = m_powerInterface->GetIdleAction(POWER_DEVICE_TYPE_BACKLIGHT,
                                                                      POWER_SUPPLY_MODE_AC);
    getMonitorIdleTimeoutReply.waitForFinished();
    if (getMonitorIdleTimeoutReply.isError())
    {
        KLOG_WARNING() << "can't get monitor idle timeout!," << getMonitorIdleTimeoutReply.error();
    }
    else
    {
        IdleAction action = getMonitorIdleTimeoutReply.value();
        findComboIdx = ui->combo_monitorOffTimeout->findData(action.idleTimeout);
        if (findComboIdx == -1)
        {
            KLOG_WARNING("can't find monitor idle timeout(%d),append it to combobox!", action.idleTimeout);
            ui->combo_monitorOffTimeout->addItem(getTimeDescription(action.idleTimeout), action.idleTimeout);
            findComboIdx = ui->combo_monitorOffTimeout->findData(action.idleTimeout);
        }
        ui->combo_monitorOffTimeout->blockSignals(true);
        ui->combo_monitorOffTimeout->setCurrentIndex(findComboIdx);
        ui->combo_monitorOffTimeout->blockSignals(false);
    }

    ///电量耗尽执行的操作
    auto eventActionRep = m_powerInterface->GetEventAction(POWER_EVENT_BATTERY_CHARGE_ACTION);
    eventActionRep.waitForFinished();
    if (eventActionRep.isError())
    {
        KLOG_WARNING() << "get lower power action failed," << eventActionRep.error();
    }
    else
    {
        int action = eventActionRep.value();
        findComboIdx = ui->combo_noPowerAction->findData(action);
        if (findComboIdx == -1)
        {
            KLOG_WARNING("can't find no power action(%d),append it to combobox!", action);
            ui->combo_noPowerAction->addItem(QString::number(action), action);
            findComboIdx = ui->combo_monitorOffTimeout->findData(action);
        }
        ui->combo_noPowerAction->blockSignals(true);
        ui->combo_noPowerAction->setCurrentIndex(findComboIdx);
        ui->combo_noPowerAction->blockSignals(false);
    }

    ///NOTE:idle dimmed scale (0关闭空闲显示变暗，其余值均为描述变暗的程度)
    bool idleDimmedEnable = (m_powerInterface->idle_dimmed_scale() != 0);
    m_reduceBrightnessSwitch->blockSignals(true);
    m_reduceBrightnessSwitch->setChecked(idleDimmedEnable);
    m_reduceBrightnessSwitch->blockSignals(false);
}

void BatterySettingsPage::handleIdleTimeActionCurrentIdxChanged(int idx)
{
    IdleAction idleAction{};
    if( !getCurrentIdleAction(idleAction) )
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

void BatterySettingsPage::handlePowerRunOutActionCurrentIdxChanged(int idx)
{
    int action = ui->combo_noPowerAction->currentData().toInt();
    auto reply = m_powerInterface->SetEventAction(POWER_EVENT_BATTERY_CHARGE_ACTION,action);

    reply.waitForFinished();

    if(reply.isError())
    {
        KLOG_WARNING() << "set no power action(" << action << ") failed," << reply.error();
    }
}

void BatterySettingsPage::handleMonitorOffTimeoutCurrentIdxChanged(int idx)
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

void BatterySettingsPage::handleSwitchReduceBrightnessToggled(bool checked)
{
    auto reply = m_powerInterface->SetIdleDimmed(checked?LOW_POWER_REDUCE_BRIGHTNESS_SCALE:0);
    reply.waitForFinished();
    if(reply.isError())
    {
        KLOG_WARNING() << "set idle dimmed" << checked << "failed," << reply.error();
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