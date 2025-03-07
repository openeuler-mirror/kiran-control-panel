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
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/WiredDevice>
#include <QList>
#include <QObject>

namespace Kiran
{
namespace Network
{
typedef NetworkManager::ConnectionSettings::ConnectionType ConnectionType;
typedef NetworkManager::WiredDevice::Ptr WiredDevicePtr;
typedef NetworkManager::Connection::Ptr ConnectionPtr;
typedef NetworkManager::Connection::List ConnectionList;

class DeviceManager;
#define CM_INSTANCE ConnectionManager::instance()
class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    static ConnectionManager* instance() { return instance_; };
    static void global_init();
    static void global_deinit() { delete instance_; };

    QString connectionActivePath(const QString& uuid);
    QStringList activedConncetions(ConnectionType type);
    bool checkConfigurationValidityForWiredDevice(QString connectionPath, WiredDevicePtr& device);
    ConnectionList wiredDeviceAvaiableConnections(WiredDevicePtr& device);
    ConnectionList availableConnections(ConnectionType type);

signals:
    void activeConnectionAdded(ConnectionType type, const QString& path, const QString& uuid);
    void activeConnectionRemoved(ConnectionType type, const QString& path, const QString& uuid);

    void connectionRemoved(const QString& connectionPath);
    void vpnConnectionAdded(const QString& connectionPath);
    void wiredConnectionAdded(const QString& connectionPath);
    void wiredConnectionSettingsChanged(const QString& connectionPath);

private:
    explicit ConnectionManager(QObject* parent = nullptr);
    void init();

private slots:
    void processConnectionAdded(const QString& connectionPath);
    void processWiredConnectionUpdate();
    void processActiveConnectionAdded(const QString& activeConnectionPath);
    void processActiveConnectionRemoved(const QString& activeConnectionPath);

private:
    static ConnectionManager* instance_;
    typedef struct ActiveConnectionInfo
    {
        ConnectionType type;
        QString uuid;
    } ActiveConnectionInfo;
    // 激活连接信息缓存 QMap<ActiveConnection::path,ActiveConnectionInfo>
    QMap<QString, ActiveConnectionInfo> m_activedConnections;
};
}  // namespace Network
}  // namespace Kiran