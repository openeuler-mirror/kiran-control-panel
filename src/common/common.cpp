//
// Created by lxh on 2021/4/27.
//

#include "common.h"
#include <QObject>

QString Common::getTimeDescription(int seconds)
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
