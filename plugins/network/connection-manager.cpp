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
#include "connection-manager.h"
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredSetting>
#include "logging-category.h"

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
static const QSet<ConnectionType> InterstingActiveConnTypeSet = {
    ConnectionSettings::Wired,
    ConnectionSettings::Wireless,
    ConnectionSettings::Vpn,
};
ConnectionManager* ConnectionManager::instance_ = nullptr;
void ConnectionManager::global_init()
{
    if (instance_ == nullptr)
    {
        instance_ = new ConnectionManager();
        instance_->init();
    }
}

QString ConnectionManager::connectionActivePath(const QString& uuid)
{
    QString connectionActivePath;
    // 通过uuid关联，连接和激活连接，返回激活连接路径
    for (auto iter = m_activedConnections.begin(); iter != m_activedConnections.end(); iter++)
    {
        auto info = iter.value();
        if (info.uuid == uuid)
        {
            connectionActivePath = iter.key();
            break;
        }
    }
    return connectionActivePath;
}

QStringList ConnectionManager::activedConncetions(ConnectionType type)
{
    QStringList activeConnections;
    for (auto iter = m_activedConnections.begin(); iter != m_activedConnections.end(); iter++)
    {
        auto info = iter.value();
        if (info.type == type)
        {
            activeConnections << iter.key();
        }
    }
    return activeConnections;
}

bool ConnectionManager::checkConfigurationValidityForWiredDevice(QString connectionPath,
                                                                 WiredDevicePtr& device)
{
    auto connection = findConnection(connectionPath);
    if (!connection)
    {
        return false;
    }

    auto settings = connection->settings();
    if (settings->connectionType() != ConnectionSettings::Wired)
    {
        return false;
    }

    auto deviceName = device->interfaceName();
    auto deviceHardwareAddr = device->permanentHardwareAddress();
    if (deviceHardwareAddr.isEmpty())
    {
        // 部分手机共享网络permanentHardwareAddress属性为空，每次插入都是随机物理地址，此时使用hardwareAddress属性
        deviceHardwareAddr = device->hardwareAddress();
    }

    // 检查配置设备名/MAC与当前设备是否匹配
    auto wiredSettings = settings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
    auto wiredSettingsMac = wiredSettings->macAddress().toHex(':').toUpper();
    auto boundInterface = settings->interfaceName();

    // 检查配置设备名是否匹配
    if (!boundInterface.isEmpty() && boundInterface != deviceName)
    {
        return false;
    }

    // 检查配置MAC是否匹配
    if (!wiredSettingsMac.isEmpty() && wiredSettingsMac != deviceHardwareAddr)
    {
        return false;
    }

    return true;
}

ConnectionList ConnectionManager::wiredDeviceAvaiableConnections(WiredDevicePtr& device)
{
    ConnectionList connections;

    if (!device)
    {
        return connections;
    }

    /**
     * NOTE:NetworkManager::availableConnections()是从org.freedesktop.NetworkManager中获取availableConnections属性。
     * NetworkManager源码中通过_nm_device_check_connection_available函数进行检查：
     * 1、如果连接配置中指定的MAC地址与当前设备的地址不一致，那么就不会出现在availableConnections列表中
     * 2、`an unrealized software device is always available, hardware devices never.`
     * 一个不可用的硬件设备是没有可用的连接。因此，但设备不可用时，仍要显示与设备相关的连接配置需要手动过滤处理。
     */
    if (device->state() > Device::Unavailable)
    {
        return device->availableConnections();
    }

    auto allConnections = NetworkManager::listConnections();
    for (auto connection : allConnections)
    {
        if (checkConfigurationValidityForWiredDevice(connection->path(), device))
            connections << connection;
    }

    return connections;
}

ConnectionList ConnectionManager::availableConnections(ConnectionType type)
{
    Connection::List connections;
    auto allConnections = NetworkManager::listConnections();
    for (auto connection : allConnections)
    {
        auto settings = connection->settings();
        if (settings && settings->connectionType() == type)
            connections << connection;
    }
    return connections;
}

ConnectionManager::ConnectionManager(QObject* parent)
{
}

void ConnectionManager::init()
{
    // 监听活跃连接（ActiveConneciton）配置变化，用于辅助判断配置是否被激活
    auto connectAddedRes = connect(notifier(), &Notifier::activeConnectionAdded, this, &ConnectionManager::processActiveConnectionAdded);
    auto connectRemovedRes = connect(notifier(), &Notifier::activeConnectionRemoved, this, &ConnectionManager::processActiveConnectionRemoved);

    auto activeConnections = NetworkManager::activeConnectionsPaths();
    for (auto connectionPath : activeConnections)
    {
        processActiveConnectionAdded(connectionPath);
    }

    // 监听连接配置变化
    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, this, &ConnectionManager::processConnectionAdded);
    connect(settingsNotifier(), &SettingsNotifier::connectionRemoved, this, &ConnectionManager::connectionRemoved);

    // 加载目前所有连接
    auto connections = NetworkManager::listConnections();
    for (auto connection : connections)
    {
        processConnectionAdded(connection->path());
    }
}

/// @brief 处理有线配置更新
/// @details 发出信号，检查有线网络配置设备绑定情况，动态新增/删除设备下可用配置列表
void ConnectionManager::processWiredConnectionUpdate()
{
    auto obj = sender();
    if (!obj)
    {
        return;
    }

    auto connection = qobject_cast<Connection*>(obj);
    if (!connection ||
        !connection->settings())
    {
        return;
    }

    auto settings = connection->settings();
    KLOG_INFO(qLcNetwork) << "wired connection update:" << connection->path() << settings->id();

    emit wiredConnectionSettingsChanged(connection->path());
}

void ConnectionManager::processActiveConnectionAdded(const QString& activeConnectionPath)
{
    auto activeConn = findActiveConnection(activeConnectionPath);
    if (!activeConn)
    {
        KLOG_INFO(qLcNetwork) << "active connection added, but active connection " << activeConnectionPath << " not found";
        return;
    }

    const auto connType = activeConn->type();
    const auto connUuid = activeConn->uuid();

    KLOG_DEBUG(qLcNetwork) << "active connection added:" << activeConnectionPath << activeConn->id() << connUuid;

    m_activedConnections[activeConnectionPath] = {.type = connType, .uuid = connUuid};
    if (InterstingActiveConnTypeSet.contains(connType))
    {
        emit activeConnectionAdded(connType, activeConnectionPath, activeConn->uuid());
    }
}
void ConnectionManager::processActiveConnectionRemoved(const QString& activeConnectionPath)
{
    if (!m_activedConnections.contains(activeConnectionPath))
    {
        KLOG_INFO(qLcNetwork) << "active connection removed, but active connection " << activeConnectionPath << " not found";
        return;
    }

    auto connInfo = m_activedConnections[activeConnectionPath];
    m_activedConnections.remove(activeConnectionPath);

    KLOG_INFO(qLcNetwork) << "active connection removed, uuid:" << connInfo.uuid << " path:" << activeConnectionPath;

    if (InterstingActiveConnTypeSet.contains(connInfo.type))
    {
        emit activeConnectionRemoved(connInfo.type, activeConnectionPath, connInfo.uuid);
    }
}
void ConnectionManager::processConnectionAdded(const QString& connectionPath)
{
    auto connection = findConnection(connectionPath);
    if (!connection)
    {
        KLOG_WARNING(qLcNetwork, "connection added,but connection %s not found", qUtf8Printable(connectionPath));
        return;
    }

    auto settings = connection->settings();

    // 忽略非法的连接配置
    if (!settings || settings->id().isEmpty())
    {
        return;
    }

    // 只处理有线配置以及VPN配置
    // WIFI界面不显示配置列表，只显示网络列表
    if (settings->connectionType() == ConnectionSettings::Wired)
    {
        connect(connection.data(), &Connection::updated, this, &ConnectionManager::processWiredConnectionUpdate, Qt::UniqueConnection);
        KLOG_INFO(qLcNetwork) << "wired connection added:" << connectionPath << settings->id();
        emit wiredConnectionAdded(connectionPath);
        return;
    }
    else if (settings->connectionType() == ConnectionSettings::Vpn)
    {
        KLOG_INFO(qLcNetwork) << "vpn connection added:" << connectionPath << settings->id();
        emit vpnConnectionAdded(connectionPath);
        return;
    }
}

}  // namespace Network
}  // namespace Kiran