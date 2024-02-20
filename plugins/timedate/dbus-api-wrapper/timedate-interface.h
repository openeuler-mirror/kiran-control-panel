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

#ifndef SYSTEMDAEMONTIMEDATE_H_1598928160
#define SYSTEMDAEMONTIMEDATE_H_1598928160

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <QDebug>

struct ZoneInfo{
  QString zone_id;
  QString zone_city;
  qint64  zone_utc;
  friend QDBusArgument& operator<<(QDBusArgument&arg,const ZoneInfo& info) {
    arg.beginStructure();
    arg << info.zone_id;
    arg << info.zone_city;
    arg << info.zone_utc;
    arg.endStructure();
    return arg;
  };
  friend const QDBusArgument& operator>>(const QDBusArgument&arg,ZoneInfo& info){
    arg.beginStructure();
    arg >> info.zone_id;
    arg >> info.zone_city;
    arg >> info.zone_utc;
    arg.endStructure();
    return arg;
  };
  bool operator== (const ZoneInfo& other) const{
    if(zone_id==other.zone_id){
      return true;
    }
    return false;
  };
  bool operator!= (const ZoneInfo& other) const{
    if(zone_id!=other.zone_id){
      return true;
    }
    return false;
  }
};
typedef QList<ZoneInfo>        TimeZoneList;
typedef QMap<QString,ZoneInfo> TimeZoneMap;

Q_DECLARE_METATYPE(ZoneInfo)
Q_DECLARE_METATYPE(TimeZoneList)

/*
 * Proxy class for interface com.kylinsec.Kiran.SystemDaemon.TimeDate
 */
class ComKylinsecKiranSystemDaemonTimeDateInterface: public QDBusAbstractInterface
{
  friend class KiranTimeDateGlobalData;
Q_OBJECT
public:
  static inline const char *staticInterfaceName()
  { return "com.kylinsec.Kiran.SystemDaemon.TimeDate"; }
  static ComKylinsecKiranSystemDaemonTimeDateInterface* instance();
  ~ComKylinsecKiranSystemDaemonTimeDateInterface();
private:
  ComKylinsecKiranSystemDaemonTimeDateInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);
private:
  Q_PROPERTY(bool can_ntp READ can_ntp)
  inline bool can_ntp() const
  { return qvariant_cast< bool >(property("can_ntp")); }

  Q_PROPERTY(bool local_rtc READ local_rtc)
  inline bool local_rtc() const
  { return qvariant_cast< bool >(property("local_rtc")); }

  Q_PROPERTY(bool ntp READ ntp)
  inline bool ntp() const
  { return qvariant_cast< bool >(property("ntp")); }

  Q_PROPERTY(qulonglong rtc_time READ rtc_time)
  inline qulonglong rtc_time() const
  { return qvariant_cast< qulonglong >(property("rtc_time")); }

  Q_PROPERTY(qulonglong system_time READ system_time)
  inline qulonglong system_time() const
  { return qvariant_cast< qulonglong >(property("system_time")); }

  Q_PROPERTY(QString time_zone READ time_zone)
  inline QString time_zone() const
  { return qvariant_cast< QString >(property("time_zone")); }

  Q_PROPERTY(int date_long_format_index READ date_long_format_index)
  inline int date_long_format_index() const
  { return qvariant_cast< int >( property("date_long_format_index") ); }

  Q_PROPERTY(int date_short_format_index READ date_short_format_index)
  inline int date_short_format_index() const
  { return qvariant_cast< int >( property("date_short_format_index") ); }

  Q_PROPERTY(int hour_format READ hour_format)
  inline int hour_format() const
  { return qvariant_cast< int >( property("hour_format") ); }

  Q_PROPERTY(bool seconds_showing READ seconds_showing)
  inline bool seconds_showing() const
  { return qvariant_cast< bool >( property("seconds_showing") ); }
private:
  QPair<bool,QString> handleEventSyncCall(const QString& methondName,const QVariantList& args){
    QPair<bool,QString> res(true,"");
    QDBusPendingCall pendingCall = asyncCallWithArgumentList(methondName,args);
    QDBusPendingCallWatcher watcher(pendingCall);
    QEventLoop loop;

    connect(&watcher,&QDBusPendingCallWatcher::finished,[&res,&loop](QDBusPendingCallWatcher*watcher){
      QDBusMessage msg = watcher->reply();
      if(msg.type()==QDBusMessage::ErrorMessage){
        res = {false,msg.errorMessage()};
      }
      loop.quit();
    });

    if(Q_UNLIKELY(pendingCall.isFinished())){
      if( pendingCall.isError() ){
        res = {false,pendingCall.error().message()};
      }
      return res;
    }

    loop.exec(QEventLoop::ExcludeUserInputEvents);

    return res;
  }
public Q_SLOTS: // METHODS
  ///---------------------------------------------------------------------------
  inline QDBusPendingReply<> SetLocalRTC(bool local, bool adjust_system)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(local) << QVariant::fromValue(adjust_system);
    return asyncCallWithArgumentList(QLatin1String("SetLocalRTC"), argumentList);
  }
  inline QPair<bool,QString> SyncSetLocalRTC(bool local,bool adjust_system)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(local) << QVariant::fromValue(adjust_system);
    return handleEventSyncCall(QLatin1String("SetLocalRTC"), argumentList);
  }
  ///---------------------------------------------------------------------------
  inline QDBusPendingReply<> SetNTP(bool active)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(active);
    return asyncCallWithArgumentList(QLatin1String("SetNTP"), argumentList);
  }
  inline QPair<bool,QString> SyncSetNTP(bool active)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(active);
    return handleEventSyncCall(QLatin1String("SetNTP"), argumentList);
  }
  ///---------------------------------------------------------------------------
  inline QDBusPendingReply<> SetTime(qlonglong requested_time, bool relative)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(requested_time) << QVariant::fromValue(relative);
    return asyncCallWithArgumentList(QLatin1String("SetTime"), argumentList);
  }
  inline QPair<bool,QString> SyncSetTime(qlonglong requested_time,bool relative)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(requested_time) << QVariant::fromValue(relative);
    return handleEventSyncCall(QLatin1String("SetTime"), argumentList);
  }
  ///---------------------------------------------------------------------------
  inline QDBusPendingReply<> SetTimezone(const QString &time_zone)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(time_zone);
    return asyncCallWithArgumentList(QLatin1String("SetTimezone"), argumentList);
  }
  inline QPair<bool,QString> SyncSetTimeZone(const QString& time_zone)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(time_zone);
    return handleEventSyncCall(QLatin1String("SetTimezone"), argumentList);
  }
  ///---------------------------------------------------------------------------
  inline QDBusPendingReply<> SetDateFormatByIndex(int type,int index)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(type) << QVariant::fromValue(index);
    return asyncCallWithArgumentList(QLatin1String("SetDateFormatByIndex"), argumentList);
  }

  inline QPair<bool,QString> SyncSetDateFormatByIndex(int type,int index)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(type) << QVariant::fromValue(index);
    return handleEventSyncCall(QLatin1String("SetDateFormatByIndex"), argumentList);
  }
  ///---------------------------------------------------------------------------
  inline QDBusPendingReply<> SetHourFormat(int format)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(format);
    return asyncCallWithArgumentList(QLatin1String("SetHourFormat"), argumentList);
  }

  inline QPair<bool,QString> SyncSetHourFormat(int format)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(format) ;
    return handleEventSyncCall(QLatin1String("SetHourFormat"), argumentList);
  }
  ///---------------------------------------------------------------------------
  inline QDBusPendingReply<> EnableSecondsShowing(bool enable)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(enable);
    return asyncCallWithArgumentList(QLatin1String("EnableSecondsShowing"), argumentList);
  }

  inline QPair<bool,QString> SyncEnableSecondsShowing(bool enable)
  {
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(enable) ;
    return handleEventSyncCall(QLatin1String("EnableSecondsShowing"), argumentList);
  }
  ///---------------------------------------------------------------------------

private slots:
  void handlePropertiesChanged(QDBusMessage msg);
  inline QDBusPendingReply<TimeZoneList> GetZoneList()
  {
    QList<QVariant> argumentList;
    return asyncCallWithArgumentList(QLatin1String("GetZoneList"), argumentList);
  }
  inline QDBusPendingReply<QStringList>  GetDateFormatList(int type)
  {
    QList<QVariant> argumentList;
    argumentList << type;
    return asyncCallWithArgumentList(QLatin1String("GetDateFormatList"),argumentList);
  }
Q_SIGNALS: // SIGNALS
  void propertyChanged(QString propertyName,QVariant value);
};

namespace com {
namespace kylinsec {
namespace Kiran {
namespace SystemDaemon {
typedef ::ComKylinsecKiranSystemDaemonTimeDateInterface TimeDate;
}
}
}
}

using com::kylinsec::Kiran::SystemDaemon::TimeDate;
#endif
