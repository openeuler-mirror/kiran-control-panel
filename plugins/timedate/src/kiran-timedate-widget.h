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

#ifndef KIRANDATETIMEWIDGET_H
#define KIRANDATETIMEWIDGET_H

#include <kiran-system-daemon/timedate-i.h>
#include <QGSettings>
#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class KiranTimeDateWidget; }
QT_END_NAMESPACE

class MaskWidget;
class KiranSwitchButton;
class QListWidgetItem;

class TimezoneSettings;
class DisplayFormatSettings;
class DateTimeSettings;

enum KiranTimeDateStackPageEnum
{
    PAGE_TIMEZONE_SETTING,
    PAGE_DATETIME_SETTING,
    PAGE_DISPLAY_FORMAT_SETTING,
    PAGE_END
};
class KiranTimeDateWidget : public QWidget
{
    Q_OBJECT

public:
    KiranTimeDateWidget(QWidget *parent = nullptr);
    ~KiranTimeDateWidget();

    QSize sizeHint() const override;

    void jumpToSubPage(KiranTimeDateStackPageEnum pageEnum);

private:
    void initUI();
    void initTimeZoneSettingsPage();
    void initDateTimeSettingsPage();
    void initDisplayFormatSettingsPage();
    void updateTimeLabel();
    void updateTimeZoneLabel();
    void setMaskWidgetVisible(bool visible);

private slots:
    void handleSidebarSelectionChanged();
    void handleAutoSyncToggled(bool checked);
    void handleSystemNTPChanged(bool ntp);
    void handleSysntemCanNTPChanged(bool canNtp);
    void handleSystemTimeZoneChanged(QString timeZone);
    void handleSystemHourFormatChanged(TimedateHourFormat hourFormat);
    void handleSystemLongDisplayFormatChanged(int idx);
    void handleSystemSecondShowingChanged(bool enable);

protected:
    virtual void timerEvent(QTimerEvent* event) override;

private:
    Ui::KiranTimeDateWidget *ui;
    int m_updateTimer = -1;
    QString m_curTimeDateFormat;
    bool m_showSeconds;
    TimedateHourFormat m_hourFormat;
    MaskWidget *m_maskWidget = nullptr;
    KiranSwitchButton *m_autoSyncSwitch = nullptr;
    TimezoneSettings *m_zoneSettingsPage = nullptr;
    DisplayFormatSettings *m_formatSettingsPage = nullptr;
    DateTimeSettings *m_dateTimeSettingsPage = nullptr;
};

#endif // KIRANDATETIMEWIDGET_H
