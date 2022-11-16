#pragma once

#include <QDBusArgument>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QSize>

enum ConfigMode
{
    CONFIG_MODE_COPY,
    CONFIG_MODE_EXTRA
};

Q_DECLARE_METATYPE(ConfigMode)

typedef QList<quint32> quint32List;
Q_DECLARE_METATYPE(quint32List)

typedef QList<quint16> quint16List;
Q_DECLARE_METATYPE(quint16List)

typedef QPair<QSize, QList<double>> modeInfoPair;
Q_DECLARE_METATYPE(modeInfoPair)

struct DisplayModesStu
{
    DisplayModesStu() : w(0), h(0), refreshRate(0) {}
    uint32_t index;
    uint32_t w;
    uint32_t h;
    double refreshRate;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const DisplayModesStu &stu)
    {
        argument.beginStructure();
        argument << stu.index << stu.w << stu.h << stu.refreshRate;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, DisplayModesStu &stu)
    {
        argument.beginStructure();
        argument >> stu.index >> stu.w >> stu.h >> stu.refreshRate;
        argument.endStructure();
        return argument;
    }

};
typedef QList<DisplayModesStu> ListDisplayModesStu;
Q_DECLARE_METATYPE(DisplayModesStu)
Q_DECLARE_METATYPE(ListDisplayModesStu)
