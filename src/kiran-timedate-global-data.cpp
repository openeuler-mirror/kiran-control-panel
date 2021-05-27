#include "kiran-timedate-global-data.h"

#include <QMutex>
#include <QDebug>

KiranTimeDateGlobalData::KiranTimeDateGlobalData(QObject *parent)
    : QObject(parent)
    , m_systemTimeZone("")
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

    QDBusPendingReply<QStringList> longDateFormatReply = TimeDate::instance()->GetDateFormatList(TIMEDATE_FORMAT_TYPE_LONG);
    longDateFormatReply.waitForFinished();
    if( longDateFormatReply.isError() ){
        qWarning() << longDateFormatReply.error();
        return false;
    }
    m_longDateFormatList = longDateFormatReply.value();
    m_longDateFormatIndex = TimeDate ::instance()->date_long_format_index();

    QDBusPendingReply<QStringList> shortDateFormatReply = TimeDate::instance()->GetDateFormatList(TIMEDATE_FORMAT_TYPE_SHORT);
    shortDateFormatReply.waitForFinished();
    if( shortDateFormatReply.isError() ){
        qWarning() << shortDateFormatReply.error();
        return false;
    }
    m_shortDateFormatList = shortDateFormatReply.value();
    m_shortDateFormatIndex = TimeDate ::instance()->date_short_format_index();

    m_hourFormat = static_cast<TimedateHourFormat>(TimeDate::instance()->hour_format());
    m_secondsShowing = TimeDate ::instance()->seconds_showing();

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

QStringList KiranTimeDateGlobalData::longDateFormatList() const {
    return m_longDateFormatList;
}

int KiranTimeDateGlobalData::longDateFormatIndex() const {
    return m_longDateFormatIndex;
}

QStringList KiranTimeDateGlobalData::shortDateFormatList() const {
    return m_shortDateFormatList;
}

int KiranTimeDateGlobalData::shortDateFormatIndex() const {
    return m_shortDateFormatIndex;
}

TimedateHourFormat KiranTimeDateGlobalData::hourFormat() const {
    return m_hourFormat;
}

bool KiranTimeDateGlobalData::secondsShowing() const {
    return m_secondsShowing;
}

void KiranTimeDateGlobalData::dumpSetting()
{
    qInfo() << "time_zone:              " << m_systemTimeZone;
    qInfo() << "local_rtc:              " << m_systemLocalRTC;
    qInfo() << "can_ntp:                " << m_systemCanNTP;
    qInfo() << "ntp:                    " << m_systemNTP;
    qInfo() << "date_long_format_index: " << m_longDateFormatIndex;
    qInfo() << "date_short_format_index:" << m_shortDateFormatIndex;
    qInfo() << "hour_format:            " << m_hourFormat;
    qInfo() << "seconds_showing:        " << m_secondsShowing;
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
    }else if(name=="date_long_format_index"){
        setLongDateFormatIndex(value.toInt());
    }else if(name=="date_short_format_index"){
        setShortDateFormatIndex(value.toInt());
    }else if(name=="hour_format"){
        TimedateHourFormat format = static_cast<TimedateHourFormat>(value.toInt());
        setHourFormat(format);
    }else if(name=="seconds_showing"){
        setSecondsShowing(value.toBool());
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

void KiranTimeDateGlobalData::setLongDateFormatIndex(int longDateFormatIndex) {
    if(m_longDateFormatIndex ==longDateFormatIndex)
        return;

    m_longDateFormatIndex = longDateFormatIndex;
    emit longDateFormatIndexChanged(m_longDateFormatIndex);
}

void KiranTimeDateGlobalData::setShortDateFormatIndex(int shortDateFormatIndex) {
    if( m_shortDateFormatIndex == shortDateFormatIndex )
        return;

    m_shortDateFormatIndex = shortDateFormatIndex;
    emit shortDateFormatIndexChanged(m_shortDateFormatIndex);
}

void KiranTimeDateGlobalData::setHourFormat(TimedateHourFormat hourFormat) {
    if( m_hourFormat == hourFormat )
        return;

    m_hourFormat = hourFormat;
    emit hourFormatChanged(m_hourFormat);
}

void KiranTimeDateGlobalData::setSecondsShowing(bool enabled) {
    if( m_secondsShowing == enabled )
        return;

    m_secondsShowing = enabled;
    emit secondsShowingChanged(m_secondsShowing);
}