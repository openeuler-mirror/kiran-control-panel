#ifndef KIRANTIMEDATEGLOBALDATA_H
#define KIRANTIMEDATEGLOBALDATA_H

#include "dbus-api-wrapper/timedate-interface.h"

#include <QObject>

class KiranTimeDateGlobalData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString systemTimeZone READ systemTimeZone NOTIFY systemTimeZoneChanged)
    Q_PROPERTY(bool systemLocalRTC READ systemLocalRTC NOTIFY systemLocalRTCChanged)
    Q_PROPERTY(bool systemCanNTP READ systemCanNTP NOTIFY systemCanNTPChanged)
    Q_PROPERTY(bool systemNTP READ systemNTP NOTIFY systemNTPChanged)
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

    void dumpSetting();
signals:
    void systemTimeZoneChanged(QString systemTimeZone);
    void systemLocalRTCChanged(bool systemLocalRTC);
    void systemCanNTPChanged(bool systemCanNTP);
    void systemNTPChanged(bool systemNTP);
private Q_SLOTS:
    void systemTimeDatePropertyChanged(QString name,QVariant value);

    void setSystemTimeZone(QString systemTimeZone);
    void setSystemLocalRTC(bool systemLocalRTC);
    void setSystemCanNTP(bool systemCanNTP);
    void setSystemNTP(bool systemNTP);
private:
    TimeZoneList m_allTimeZoneList;
    TimeZoneMap m_timeZoneIDMap;
    QString m_systemTimeZone;
    bool m_systemLocalRTC;
    bool m_systemCanNTP;
    bool m_systemNTP;
};

#endif // KIRANTIMEDATEGLOBALDATA_H
