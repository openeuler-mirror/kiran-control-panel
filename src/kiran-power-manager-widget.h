//
// Created by lxh on 2021/4/6.
//

#ifndef KIRAN_POWER_MANAGER_SRC_KIRAN_POWER_MANAGER_WIDGET_H_
#define KIRAN_POWER_MANAGER_SRC_KIRAN_POWER_MANAGER_WIDGET_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class KiranPowerManagerWidget;
}
QT_END_NAMESPACE

class GeneralSettingsPage;
class PowerSettingsPage;
class BatterySettingsPage;
class PowerInterface;
class KiranPowerManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranPowerManagerWidget(QWidget *parent = nullptr);
    ~KiranPowerManagerWidget() override;

private:
    void init();
    void initUI();
    void initConnection();
    void loadStyleSheet();

private:
    Ui::KiranPowerManagerWidget *ui;
    GeneralSettingsPage* m_generalSettings;
    PowerSettingsPage* m_powerSettings;
    BatterySettingsPage* m_batterySettings;
    PowerInterface* m_powerInterface;
};

#endif  //KIRAN_POWER_MANAGER_SRC_KIRAN_POWER_MANAGER_WIDGET_H_
