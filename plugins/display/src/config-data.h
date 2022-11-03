#pragma once

#include <kiran-session-daemon/display-i.h>
#include <QObject>
#include "generic-types.h"

Q_DECLARE_FLAGS(DisplayReflectTypes, DisplayReflectType)
Q_DECLARE_OPERATORS_FOR_FLAGS(DisplayReflectTypes);

class DisplayConfigData : public QObject
{
    Q_OBJECT
public:
    explicit DisplayConfigData(const QString &displayPath);

    QString path();
    QString primary();
    void setPrimary(const QString &primary);

    int windowScalingFactor();
    void setWindowScalingFactor(int windowSaclingFactor);

private:
    QString m_displayPath;
    QString m_primary;
    int m_windowScalingFactor;
};

class MonitorConfigData : public QObject
{
    Q_OBJECT
public:
    MonitorConfigData(const QString &monitorPath);

    QString path();

    uint id();
    void setId(uint id);

    QString name();
    void setName(const QString &name);

    int x();
    void setX(int x);

    int y();
    void setY(int y);

    int w();
    void setW(int w);

    int h();
    void setH(int h);

    QSize resolving();
    void setResolving(const QSize &size);

    double refreshRate();
    void setRefreshRate(double refreshRate);

    DisplayRotationType rotation();
    void setRotation(DisplayRotationType rotation);

    DisplayReflectTypes reflect();
    void setReflect(const DisplayReflectTypes &reflect);

    bool enabled();
    void setEnabled(bool enable);

    DisplayModesStu modesStu();
    void setModesStu(DisplayModesStu modesStu);

signals:
    void resolvingChanged(const QSize &size);

private:
    QString m_monitorPath;
    uint m_id;
    QString m_name;

    bool m_connected;
    bool m_enabled;
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    DisplayRotationType m_rotation;
    DisplayReflectTypes m_reflect;
    DisplayModesStu m_modesStu;
};
