/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
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
#include <QSettings>
#include "config.h"
#include "network-prefs.h"

namespace Kiran
{
namespace Network
{
Prefs *Prefs::instance()
{
    static Prefs *instance = nullptr;

    if (!instance)
    {
        instance = new Prefs;
    }

    return instance;
}

Prefs::Prefs()
{
    QSettings settings(SETTINGS_PATH, QSettings::NativeFormat);

    // 是否开启联通性检查
    QVariant checkConnectivityVar = settings.value(QString("Network/CheckInternetConnectivity"), true);
    m_checkConnectivity = checkConnectivityVar.toBool();
    if (m_checkConnectivity)
    {
        QVariant addressVar = settings.value(QString("Network/Address"));
        m_connectivityAddress = addressVar.toString();

        QVariant portVar = settings.value(QString("Network/Port"));
        m_connectivityPort = portVar.toInt();
    }

    // 检查网线网口负载,作为托盘显示连接状态的一个依据，CYSR定制需求(#36646)
    QVariant checkWiredCarrier = settings.value(QString("Network/CheckWiredCarrier"), false);
    m_checkWiredCarrier = checkWiredCarrier.toBool();
}

Prefs::~Prefs()
{
}

bool Prefs::getCheckConnectivity() const
{
    return m_checkConnectivity;
}

QString Prefs::getConnectivityAddress() const
{
    return m_connectivityAddress;
}

qint64 Prefs::getConnectivityPort() const
{
    return m_connectivityPort;
}

bool Prefs::getCheckWiredCarrier() const
{
    return m_checkWiredCarrier;
}
}  // namespace Network
}  // namespace Kiran