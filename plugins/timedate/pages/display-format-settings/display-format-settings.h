/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
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

#ifndef KIRAN_CPANEL_DATETIME_SRC_PAGES_DISPLAY_FORMAT_DISPLAY_FORMAT_SETTINGS_H_
#define KIRAN_CPANEL_DATETIME_SRC_PAGES_DISPLAY_FORMAT_DISPLAY_FORMAT_SETTINGS_H_

#include <QWidget>
#include <kiran-system-daemon/timedate-i.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DisplayFormatSettings;
}
QT_END_NAMESPACE

class KiranSwitchButton;
class DisplayFormatSettings : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayFormatSettings(QWidget *parent = nullptr);
    ~DisplayFormatSettings() override;

private:
    void init();

private slots:
    void handleLongDisplayFormatChanged(int idx);
    void handleSystemLongDisplayFormatChanged(int idx);

    void handleShortgDisplayFormatChanged(int idx);
    void handleSystemShortDisplayFormatChanged(int idx);

    void handleHourFormatChanged(int idx);
    void handleSystemHourFormatChanged(TimedateHourFormat format);

    void handleTimeSecondVisibleChanged(bool enable);
    void handleSystemTimeSecondVisibleChanged(bool enable);

private:
    Ui::DisplayFormatSettings *ui;
    KiranSwitchButton *m_showSecondSwitch= nullptr;
};

#endif  //KIRAN_CPANEL_DATETIME_SRC_PAGES_DISPLAY_FORMAT_DISPLAY_FORMAT_SETTINGS_H_
