#include "config-data.h"
#include <QSize>

DisplayConfigData::DisplayConfigData(const QString &displayPath)
{
    m_displayPath = displayPath;
}

QString DisplayConfigData::path()
{
    return m_displayPath;
}

QString DisplayConfigData::primary()
{
    return m_primary;
}

void DisplayConfigData::setPrimary(const QString &primary)
{
    m_primary = primary;
}

int DisplayConfigData::windowScalingFactor()
{
    return m_windowScalingFactor;
}

void DisplayConfigData::setWindowScalingFactor(int windowSaclingFactor)
{
    m_windowScalingFactor = windowSaclingFactor;
}

MonitorConfigData::MonitorConfigData(const QString &monitorPath)
{
    m_monitorPath = monitorPath;
}

QString MonitorConfigData::path()
{
    return m_monitorPath;
}

void MonitorConfigData::setId(uint id)
{
    m_id = id;
}

uint MonitorConfigData::id()
{
    return m_id;
}

void MonitorConfigData::setName(const QString &name)
{
    m_name = name;
}

QString MonitorConfigData::name()
{
    return m_name;
}

int MonitorConfigData::x()
{
    return m_x;
}

void MonitorConfigData::setX(int x)
{
    m_x = x;
}

int MonitorConfigData::y()
{
    return m_y;
}

void MonitorConfigData::setY(int y)
{
    m_y = y;
}

int MonitorConfigData::w()
{
    return m_modesStu.w;
}

void MonitorConfigData::setW(int w)
{
    m_modesStu.w = w;
}

int MonitorConfigData::h()
{
    return m_modesStu.h;
}

void MonitorConfigData::setH(int h)
{
    m_modesStu.h = h;
}

QSize MonitorConfigData::resolving()
{
    return QSize(m_modesStu.w, m_modesStu.h);
}

void MonitorConfigData::setResolving(const QSize &size)
{
    m_modesStu.w = size.width();
    m_modesStu.h = size.height();
    emit resolvingChanged(size);
}

double MonitorConfigData::refreshRate()
{
    return m_modesStu.refreshRate;
}

void MonitorConfigData::setRefreshRate(double refreshRate)
{
    m_modesStu.refreshRate = refreshRate;
}

DisplayRotationType MonitorConfigData::rotation()
{
    return m_rotation;
}

void MonitorConfigData::setRotation(DisplayRotationType rotation)
{
    m_rotation = rotation;
}

DisplayReflectTypes MonitorConfigData::reflect()
{
    return m_reflect;
}

void MonitorConfigData::setReflect(const DisplayReflectTypes &reflect)
{
    m_reflect = reflect;
}

bool MonitorConfigData::enabled()
{
    return m_enabled;
}

void MonitorConfigData::setEnabled(bool enable)
{
    m_enabled = enable;
}

DisplayModesStu MonitorConfigData::modesStu()
{
    return m_modesStu;
}

void MonitorConfigData::setModesStu(DisplayModesStu modesStu)
{
    m_modesStu = modesStu;
}
