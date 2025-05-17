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
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/WiredDevice>

class QMouseEvent;

namespace Kiran
{
namespace Network
{
class AnimationLoadingLabel;
class WiredSettingsItem : public ConnectionItem
{
    Q_OBJECT
public:
    explicit WiredSettingsItem(NetworkManager::WiredDevice::Ptr& device,
                               NetworkManager::Connection::Ptr& connection,
                               QWidget* parent = nullptr);
    ~WiredSettingsItem();

    QString name() const { return m_connectionName; };
    QString uuid() const { return m_connectionUUID; };
    QString path() const { return m_connectionPath; };

private:
    void init();

private slots:
    void updateActivatedState();
    void disconnectConnectionSettings();
    void connectConnectionSettings();
    void removeConnectionSettings();
    void showAdvancedSettings();
    void updateConnectionName();

private:
    NetworkManager::Device::Ptr m_device;
    NetworkManager::Connection::Ptr m_connection;
    QString m_connectionName;
    QString m_connectionUUID;
    QString m_connectionPath;
};
}  // namespace Network
}  // namespace Kiran