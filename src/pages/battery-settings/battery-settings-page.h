//
// Created by lxh on 2021/4/6.
//

#ifndef KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_
#define KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class BatterySettingsPage;
}
QT_END_NAMESPACE

class BatterySettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit BatterySettingsPage(QWidget *parent = nullptr);
    ~BatterySettingsPage() override;

private:
    void init();
    void initUI();
    void initConnection();
    void load();

private:
    Ui::BatterySettingsPage *ui;
};

#endif  //KIRAN_POWER_MANAGER_SRC_PAGES_BATTERY_SETTINGS_BATTERY_SETTINGS_PAGE_H_
