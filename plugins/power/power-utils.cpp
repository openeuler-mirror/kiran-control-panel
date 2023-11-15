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

#include "power-utils.h"
#include <kiran-session-daemon/power-i.h>
#include <QObject>

QString PowerUtils::getTimeDescription(int seconds)
{
    static const int minute_unit = 60;
    static const int hour_unit = minute_unit * 60;
    static const int day_unit = hour_unit * 24;

    int day = seconds / day_unit;
    int hour = (seconds - (day * day_unit)) / hour_unit;
    int minute = (seconds - (day * day_unit) - (hour * hour_unit)) / minute_unit;

    QString formatTime;
    if (day != 0)
    {
        formatTime.append(QString(QObject::tr("%1Day")).arg(day));
    }
    if (hour != 0)
    {
        formatTime.append(QString(QObject::tr("%1Hour")).arg(hour));
    }
    if (minute != 0)
    {
        formatTime.append(QString(QObject::tr("%1Minute")).arg(minute));
    }
    if (formatTime.isEmpty())
    {
        formatTime.append(QObject::tr("never"));
    }
    return formatTime;
}

QString PowerUtils::powerActionDesc(int powerAction)
{
    switch (powerAction)
    {
    case POWER_ACTION_DISPLAY_ON:
        return "Display On";
    case POWER_ACTION_DISPLAY_STANDBY:
        return "Display StandBy";
    case POWER_ACTION_DISPLAY_SUSPEND:
        return "Display Suspend";
    case POWER_ACTION_DISPLAY_OFF:
        return "Display Off";
    case POWER_ACTION_COMPUTER_SUSPEND:
        return "Computer Suspend";
    case POWER_ACTION_COMPUTER_SHUTDOWN:
        return "Computer Shutdown";
    case POWER_ACTION_COMPUTER_HIBERNATE:
        return "Computer Hibernate";
    case POWER_ACTION_NOTHING:
        return "Do Nothing";
    default:
        return "Unknow";
    };
}
