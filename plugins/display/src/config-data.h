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
#pragma once

#include <kiran-session-daemon/display-i.h>
#include <QObject>
#include <QPoint>
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

    QPoint position() { return QPoint(m_x, m_y); };
    void setPosition(int x, int y)
    {
        m_x = x;
        m_y = y;
    };

    void setX(int x) { m_x = x; };
    void setY(int y) { m_y = y; };

    int width() { return m_modesStu.w; };
    void setWidth(int w) { m_modesStu.w = w; };

    int height() { return m_modesStu.h; };
    void setHeight(int h) { m_modesStu.h = h; };

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
    uint m_id = 0;
    QString m_name;

    bool m_enabled = true;
    int m_x = 0;
    int m_y = 0;
    int m_w = 0;
    int m_h = 0;
    DisplayRotationType m_rotation;
    DisplayReflectTypes m_reflect;
    DisplayModesStu m_modesStu;
};
