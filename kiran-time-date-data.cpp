#include "kiran-time-date-data.h"

#include <QMutex>
#include <QScopedPointer>
#include <QDBusConnection>

KiranTimeDateData::KiranTimeDateData(QObject *parent) : QObject(parent)
{
    QDBusPendingReply<TimeZoneList> pendingReply = ComUnikylinKiranSystemDaemonTimeDateInterface::instance()->GetZoneList();
    pendingReply.waitForFinished();
    QDBusMessage reply =pendingReply.reply();
    if(reply.type()!=QDBusMessage::ErrorMessage){
        m_timeZoneList = pendingReply.value();
        qSort(m_timeZoneList.begin(),m_timeZoneList.end(),[](const ZoneInfo& z1,const ZoneInfo& z2){
            return z1.zone_id.toLower() < z2.zone_id.toLower();
        });
        foreach (auto iter, m_timeZoneList) {
            m_timeZoneMap.insert(iter.zone_id,iter);
        }
    }
}

KiranTimeDateData *KiranTimeDateData::instance()
{
    static QMutex mutex;
    static QScopedPointer<KiranTimeDateData> pInst;

    if(Q_UNLIKELY(!pInst)){
        QMutexLocker locker(&mutex);
        if(pInst.isNull()){
            pInst.reset(new KiranTimeDateData);
        }
    }

    return pInst.data();
}

KiranTimeDateData::~KiranTimeDateData()
{

}

TimeZoneList::ConstIterator KiranTimeDateData::timeZoneListStart()
{
    return m_timeZoneList.begin();
}

TimeZoneList::ConstIterator KiranTimeDateData::timeZoneListEnd()
{
    return m_timeZoneList.end();
}

int KiranTimeDateData::timeZoneListSize()
{
    return m_timeZoneList.size();
}

bool KiranTimeDateData::getZoneInfoByID(const QString &zoneID, ZoneInfo &zoneInfo)
{
    auto iter = m_timeZoneMap.find(zoneID);
    if(iter==m_timeZoneMap.end()){
        return false;
    }
    zoneInfo = iter.value();
    return true;
}
