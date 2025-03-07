/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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
#pragma once
#include "connection-item.h"
#include <wireless-network-manager.h>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/WirelessDevice>

class QAction;
class QDBusPendingCallWatcher;
namespace Kiran
{
namespace Network
{
class WirelessNetworkItem : public ConnectionItem
{
    Q_OBJECT
public:
    explicit WirelessNetworkItem(NetworkManager::WirelessDevice::Ptr& device,
                                 WirelessNetworkManager* networkManager,
                                 WirelessNetworkInfo networkInfo,
                                 QWidget* parent = nullptr);
    ~WirelessNetworkItem();

    QString ssid() const { return m_ssid; };
    int signalStrength() const { return m_signalStrength; };

private:
    void init(WirelessNetworkInfo networkInfo);
    QString getNetworkSignalIcon(int signalPercent, bool isSecure);
    void  activateHiddenNetwork();

private slots:
    void updateActivatedState();
    void ignoreNetwork();
    void showAdvancedSettings();
    void deactivateNetwork();
    void tryActivateNetwork();

private:
    NetworkManager::WirelessDevice::Ptr m_device;
    WirelessNetworkManager* m_networkManager = nullptr;
    QString m_ssid;
    int m_signalStrength = 0;
    QString m_referencePointPath;
    QAction* m_ignoreAction = nullptr;
    QAction* m_advancedSettingsAction = nullptr;
    bool m_isSecure = false;
};
}  // namespace Network
}  // namespace Kiran