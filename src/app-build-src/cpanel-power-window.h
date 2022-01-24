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

#ifndef KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_CPANEL_POWER_WINDOW_H_
#define KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_CPANEL_POWER_WINDOW_H_

#include <kiran-titlebar-window.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CPanelPowerWindow;
}
QT_END_NAMESPACE

class CPanelPowerWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit CPanelPowerWindow(QWidget *parent = nullptr);
    ~CPanelPowerWindow() override;

private:
    Ui::CPanelPowerWindow *ui;
};

#endif  //KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_CPANEL_POWER_WINDOW_H_
