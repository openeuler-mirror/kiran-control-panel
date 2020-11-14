#include "kiran-timedate-global-data.h"

#include <QMutex>
#include <QDebug>

KiranTimeDateGlobalData::KiranTimeDateGlobalData(QObject *parent)
    : QObject(parent)
    , m_systemTimeZone("")
    , m_systemLocalRTC(false)
    , m_systemCanNTP(false)
    , m_systemNTP(false)
{

}

KiranTimeDateGlobalData *KiranTimeDateGlobalData::instance()
{
    static QMutex mutex;
    static QScopedPointer<KiranTimeDateGlobalData> pInst;

    if(Q_UNLIKELY(!pInst)){
        QMutexLocker locker(&mutex);
        if(pInst.isNull()){
            pInst.reset(new KiranTimeDateGlobalData);
        }
    }

    return pInst.data();
}

KiranTimeDateGlobalData::~KiranTimeDateGlobalData()
{

}

bool KiranTimeDateGlobalData::init()
{
    QDBusPendingReply<TimeZoneList> getZoneListReply = TimeDate::instance()->GetZoneList();
    getZoneListReply.waitForFinished();
    if(getZoneListReply.isError()){
        qWarning() << getZoneListReply.error();
        return false;
    }

    m_allTimeZoneList = getZoneListReply.value();
    std::sort(m_allTimeZoneList.begin(),m_allTimeZoneList.end(),[](const ZoneInfo& z1,const ZoneInfo& z2){
        return z1.zone_id.toLower() < z2.zone_id.toLower();
    });

    m_timeZoneIDMap.clear();
    for(auto iter=m_allTimeZoneList.begin();
        iter!=m_allTimeZoneList.end();
        iter++){
        m_timeZoneIDMap[iter->zone_id] = *iter;
    }

    m_systemTimeZone = TimeDate::instance()->time_zone();
    m_systemLocalRTC = TimeDate::instance()->local_rtc();
    m_systemCanNTP = TimeDate::instance()->can_ntp();
    m_systemNTP = TimeDate::instance()->ntp();

    qInfo() << "KiranTimeDateGlobalData init success";
    dumpSetting();

    connect(TimeDate::instance(),&TimeDate::propertyChanged,this,&KiranTimeDateGlobalData::systemTimeDatePropertyChanged);
    return true;
}

TimeZoneList::ConstIterator KiranTimeDateGlobalData::allTimeZoneListBeginIter()
{
    return m_allTimeZoneList.constBegin();
}

TimeZoneList::ConstIterator KiranTimeDateGlobalData::allTimeZoneListEndIter()
{
    return m_allTimeZoneList.constEnd();
}

int KiranTimeDateGlobalData::allTimeZoneListSize()
{
    return m_allTimeZoneList.size();
}

bool KiranTimeDateGlobalData::findZoneInfoByZoneID(QString zoneID, ZoneInfo &info)
{
    auto iter = m_timeZoneIDMap.find(zoneID);
    if(iter==m_timeZoneIDMap.end()){
        return false;
    }
    info = iter.value();
    return true;
}

QString KiranTimeDateGlobalData::systemTimeZone() const
{
    return m_systemTimeZone;
}

bool KiranTimeDateGlobalData::systemLocalRTC() const
{
    return m_systemLocalRTC;
}

bool KiranTimeDateGlobalData::systemCanNTP() const
{
    return m_systemCanNTP;
}

bool KiranTimeDateGlobalData::systemNTP() const
{
    return m_systemNTP;
}

void KiranTimeDateGlobalData::dumpSetting()
{
    qInfo() << "time_zone:   " << m_systemTimeZone;
    qInfo() << "local_rtc:   " << m_systemLocalRTC;
    qInfo() << "can_ntp:     " << m_systemCanNTP;
    qInfo() << "ntp:         " << m_systemNTP;
}

void KiranTimeDateGlobalData::systemTimeDatePropertyChanged(QString name, QVariant value)
{
    if(name=="time_zone"){
        setSystemTimeZone(value.toString());
    }else if(name=="local_rtc"){
        setSystemLocalRTC(value.toBool());
    }else if(name=="can_ntp"){
        setSystemCanNTP(value.toBool());
    }else if(name=="ntp"){
        setSystemNTP(value.toBool());
    }
}

void KiranTimeDateGlobalData::setSystemTimeZone(QString systemTimeone)
{
    if (m_systemTimeZone == systemTimeone)
        return;
    m_systemTimeZone = systemTimeone;
    emit systemTimeZoneChanged(m_systemTimeZone);
}

void KiranTimeDateGlobalData::setSystemLocalRTC(bool systemLocalRTC)
{
    if (m_systemLocalRTC == systemLocalRTC)
        return;
    m_systemLocalRTC = systemLocalRTC;
    emit systemLocalRTCChanged(m_systemLocalRTC);
}

void KiranTimeDateGlobalData::setSystemCanNTP(bool systemCanNTP)
{
    if (m_systemCanNTP == systemCanNTP)
        return;

    m_systemCanNTP = systemCanNTP;
    emit systemCanNTPChanged(m_systemCanNTP);
}

void KiranTimeDateGlobalData::setSystemNTP(bool systemNTP)
{
    if (m_systemNTP == systemNTP)
        return;

    m_systemNTP = systemNTP;
    emit systemNTPChanged(m_systemNTP);
}
