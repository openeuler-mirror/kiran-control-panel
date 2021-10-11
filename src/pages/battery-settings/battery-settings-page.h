//
// Created by lxh on 2021/4/6.
//

#ifndef KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_
#define KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_

#include <QWidget>
#include "power.h"

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
    void handleIdleTimeActionCurrentIdxChanged(int idx);
    void handlePowerRunOutActionCurrentIdxChanged(int idx);
    void handleMonitorOffTimeoutCurrentIdxChanged(int idx);
    void handleSwitchReduceBrightnessToggled(bool checked);

private:
    Ui::BatterySettingsPage *ui;
    KiranSwitchButton *m_reduceBrightnessSwitch;
    PowerInterface *m_powerInterface;
};

#endif  //KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_
