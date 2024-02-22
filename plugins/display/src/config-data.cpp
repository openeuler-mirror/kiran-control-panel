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
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */
#include "config-data.h"
#include <QSize>

DisplayConfigData::DisplayConfigData(const QString &displayPath)
    : m_displayPath(displayPath),
      m_windowScalingFactor(0)
{
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

MonitorConfigData::MonitorConfigData(const QString &monitorPath) : m_monitorPath(monitorPath)
{
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
