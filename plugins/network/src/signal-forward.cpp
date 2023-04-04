/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
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

#include "signal-forward.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QMutex>
using namespace NetworkManager;

SignalForward *SignalForward::instance()
{
    static QMutex mutex;
    static QScopedPointer<SignalForward> pInst;
    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new SignalForward());
        }
    }
    return pInst.data();
}

SignalForward::SignalForward(QObject *parent)
{
    initConnect();
};

SignalForward::~SignalForward()
{
}

void SignalForward::initConnect()
{
    //连接Wired时触发，而连接VPN时没有触发该信号，暂时不使用该信号
    // connect(notifier(), &Notifier::statusChanged, this, [this](NetworkManager::Status status) {});

    // activeConnectionAdded信号并不能判断连接是否真正Connected/Activated,只能判断一个连接被加入到激活容器中
    connect(notifier(), &Notifier::activeConnectionAdded, this, &SignalForward::handleActiveConnectionAdded, Qt::UniqueConnection);
    connect(notifier(), &Notifier::activeConnectionRemoved, this, &SignalForward::handleActiveConnectionRemoved, Qt::UniqueConnection);
    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, this, &SignalForward::handleNotifierConnectionAdded, Qt::UniqueConnection);
    connect(settingsNotifier(), &SettingsNotifier::connectionRemoved, this, &SignalForward::handleNotifierConnectionRemoved, Qt::UniqueConnection);
}

void SignalForward::handleActiveConnectionAdded(const QString &activePath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activePath);
    if (activeConnection.isNull())
        return;

    auto type = activeConnection->type();
    switch (type)
    {
    case ConnectionSettings::ConnectionType::Wired:
        Q_EMIT wiredActiveConnectionAdded(activePath);
        break;
    case ConnectionSettings::ConnectionType::Vpn:
        Q_EMIT vpnActiveConnectionAdded(activePath);
        break;
    case ConnectionSettings::ConnectionType::Wireless:
        Q_EMIT wirelessActiveConnectionAdded(activePath);
        break;
    default:
        break;
    }
}

void SignalForward::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection = NetworkManager::findConnection(path);
    auto connectionType = connection->settings()->connectionType();
    if (!connection->name().isEmpty())
    {
        switch (connectionType)
        {
        case ConnectionSettings::ConnectionType::Wired:
            Q_EMIT wiredConnectionAdded(path);
            break;
        case ConnectionSettings::ConnectionType::Vpn:
            Q_EMIT vpnConnectionAdded(path);
            break;
        case ConnectionSettings::ConnectionType::Wireless:
            Q_EMIT wirelessConnectionAdded(path);
            break;
        default:
            break;
        }
    }
}

void SignalForward::handleNotifierConnectionRemoved(const QString &path)
{
    Q_EMIT connectionRemoved(path);
}

void SignalForward::handleActiveConnectionRemoved(const QString &activepath)
{
    Q_EMIT activeConnectionRemoved(activepath);
}