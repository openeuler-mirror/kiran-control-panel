#ifndef KIRANTIMEDATEDATA_H
#define KIRANTIMEDATEDATA_H

#include <QObject>

#include "SystemDaemonTimeDate.h"
class KiranTimeDateData : public QObject
{
    Q_OBJECT
private:
    explicit KiranTimeDateData(QObject *parent = nullptr);
public:
    static KiranTimeDateData* instance();
    ~KiranTimeDateData();

    TimeZoneList::ConstIterator timeZoneListStart();
    TimeZoneList::ConstIterator timeZoneListEnd();
    int timeZoneListSize();

    bool getZoneInfoByID(const QString& zoneID,ZoneInfo& zoneInfo);
private:
    TimeZoneList m_timeZoneList;
    TimeZoneMap m_timeZoneMap;
};

#endif // KIRANTIMEDATEDATA_H
