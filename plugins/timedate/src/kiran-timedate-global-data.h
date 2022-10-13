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

#ifndef KIRANTIMEDATEGLOBALDATA_H
#define KIRANTIMEDATEGLOBALDATA_H

#include "dbus-api-wrapper/timedate-interface.h"

#include <QObject>
#include <kiran-system-daemon/timedate-i.h>

class KiranTimeDateGlobalData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString systemTimeZone READ systemTimeZone NOTIFY systemTimeZoneChanged)
    Q_PROPERTY(bool systemLocalRTC READ systemLocalRTC NOTIFY systemLocalRTCChanged)
    Q_PROPERTY(bool systemCanNTP READ systemCanNTP NOTIFY systemCanNTPChanged)
    Q_PROPERTY(bool systemNTP READ systemNTP NOTIFY systemNTPChanged)

    Q_PROPERTY(int  longDateFormatIndex READ longDateFormatIndex NOTIFY longDateFormatIndexChanged)
    Q_PROPERTY(int  shortDateFormatIndex READ shortDateFormatIndex NOTIFY shortDateFormatIndexChanged)
    Q_PROPERTY(TimedateHourFormat hourFormat READ hourFormat NOTIFY hourFormatChanged)
    Q_PROPERTY(bool secondsShowing READ secondsShowing NOTIFY secondsShowingChanged)

private:
    explicit KiranTimeDateGlobalData(QObject *parent = nullptr);

public:
    static KiranTimeDateGlobalData* instance();

    ~KiranTimeDateGlobalData();

    bool init();

    TimeZoneList::ConstIterator allTimeZoneListBeginIter();
    TimeZoneList::ConstIterator allTimeZoneListEndIter();
    int allTimeZoneListSize();

    bool findZoneInfoByZoneID(QString zoneID,ZoneInfo& info);

    QString systemTimeZone() const;
    bool systemLocalRTC() const;
    bool systemCanNTP() const;
    bool systemNTP() const;

    QStringList longDateFormatList() const;
    int longDateFormatIndex() const;
    QStringList shortDateFormatList() const;
    int shortDateFormatIndex() const;
    TimedateHourFormat hourFormat() const;
    bool secondsShowing() const;

    void dumpSetting();

signals:
    void systemTimeZoneChanged(QString systemTimeZone);
    void systemLocalRTCChanged(bool systemLocalRTC);
    void systemCanNTPChanged(bool systemCanNTP);
    void systemNTPChanged(bool systemNTP);

    void longDateFormatIndexChanged(int idx);
    void shortDateFormatIndexChanged(int idx);
    void hourFormatChanged(TimedateHourFormat format);
    void secondsShowingChanged(bool enabled);

private Q_SLOTS:
    void systemTimeDatePropertyChanged(QString name,QVariant value);

    void setSystemTimeZone(QString systemTimeZone);
    void setSystemLocalRTC(bool systemLocalRTC);
    void setSystemCanNTP(bool systemCanNTP);
    void setSystemNTP(bool systemNTP);

    void setLongDateFormatIndex(int longDateFormatIndex);
    void setShortDateFormatIndex(int shortDateFormatIndex);
    void setHourFormat(TimedateHourFormat hourFormat);
    void setSecondsShowing(bool enabled);

private:
    TimeZoneList m_allTimeZoneList;
    TimeZoneMap m_timeZoneIDMap;
    QString m_systemTimeZone;
    bool m_systemLocalRTC;
    bool m_systemCanNTP;
    bool m_systemNTP;
    int m_longDateFormatIndex = -1;
    QStringList m_longDateFormatList;
    int m_shortDateFormatIndex = -1;
    QStringList m_shortDateFormatList;
    TimedateHourFormat m_hourFormat = TIMEDATE_HOUSR_FORMAT_24_HOURS;
    bool m_secondsShowing = false;
};

#endif // KIRANTIMEDATEGLOBALDATA_H
