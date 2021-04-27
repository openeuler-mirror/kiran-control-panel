//
// Created by lxh on 2021/4/6.
//

#ifndef _CPANEL_POWER_WIDGET_H_
#define _CPANEL_POWER_WIDGET_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CPanelPowerWidget;
}
QT_END_NAMESPACE

class GeneralSettingsPage;
class PowerSettingsPage;
class BatterySettingsPage;
class PowerInterface;
class CPanelPowerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CPanelPowerWidget(QWidget* parent = nullptr);
    ~CPanelPowerWidget() override;

private:
    void init();
    void initUI();
    void initConnection();
    void loadStyleSheet();

private:
    Ui::CPanelPowerWidget* ui;
    GeneralSettingsPage* m_generalSettings = nullptr;
    PowerSettingsPage* m_powerSettings = nullptr;
    BatterySettingsPage* m_batterySettings = nullptr;
    PowerInterface* m_powerInterface = nullptr;
};

#endif  //_CPANEL_POWER_WIDGET_H_
