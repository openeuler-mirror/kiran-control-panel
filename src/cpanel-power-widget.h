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
