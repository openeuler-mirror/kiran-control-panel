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
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#include "manager.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include "signal-forward.h"

using namespace NetworkManager;

Manager::Manager(QWidget *parent) : QWidget(parent)
{
}

Manager::~Manager()
{
}

void Manager::refreshConnectionLists()
{
    KLOG_DEBUG() << "Manager::refreshConnectionLists()";
}

void Manager::handleActiveConnectionStateChanged(ActiveConnection::State state)
{
    auto activeConnection = qobject_cast<ActiveConnection *>(sender());
    QString path = activeConnection->path();
    switch (state)
    {
    case ActiveConnection::State::Unknown:
        KLOG_DEBUG() << "ActiveConnection::State::Unknown";
        break;
    case ActiveConnection::State::Activating:
        KLOG_DEBUG() << "ActiveConnection::State::Activating";
        handleStateActivating(path);
        break;
    case ActiveConnection::State::Activated:
        KLOG_DEBUG() << "ActiveConnection::State::Activated";
        handleStateActivated(path);
        break;
    case ActiveConnection::State::Deactivating:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivating";
        break;
    case ActiveConnection::State::Deactivated:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivated";
        handleStateDeactivated(path);
        break;
    default:
        break;
    }
}

void Manager::handleDeviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
}