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

#ifndef KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_
#define KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_

#include <QWidget>
#include "dbus/power.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class BatterySettingsPage;
}
QT_END_NAMESPACE

class KiranSwitchButton;
class PowerInterface;
class BatterySettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit BatterySettingsPage(QWidget *parent = nullptr);
    ~BatterySettingsPage() override;
    virtual QSize sizeHint() const override;

private:
    void init();
    void initUI();
    void initConnection();
    void load();
    bool getCurrentIdleAction(IdleAction& idleAction);

private slots:
    void onIdleTimeOrActionChanged(int idx);
    void onBatteryChargeActionChanged(int idx);
    void onMonitorOffIdleTimeoutChanged(int idx);
    void onDisplayIdleDimmedToggled(bool checked);
    void onChargeLowDimmedToggled(bool checked);
    void onChargeLowSaverToggled(bool checked);

private:
    Ui::BatterySettingsPage *ui;
    KiranSwitchButton *m_switchIdleDimmed;
    KiranSwitchButton *m_switchChargeLowDimmed;
    KiranSwitchButton *m_chargeLowSaverSwitch;
    PowerInterface *m_powerInterface;
};

#endif  //KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_
