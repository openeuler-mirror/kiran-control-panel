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
#include <NetworkManagerQt/Device>
#include <QObject>
#include <QSet>

namespace Kiran
{
namespace Network
{
#define DM_INSTANCE DeviceManager::instance()
class DeviceManager : public QObject
{
    Q_OBJECT
public:
    static DeviceManager* instance() { return instance_; };
    static void global_init();
    static void global_deinit() { delete instance_; };

    // 获取当前接管可用的网卡设备列表
    QStringList managedWiredDeivces();
    QStringList managedWirelessDeivces();

signals:
    void managedDeviceChanged();
    void managedWiredDeviceAdded(const QString& devicePath);
    void managedWiredDeviceRemoved(const QString& devicePath);
    void managedWirelessDeviceAdded(const QString& devicePath);
    void managedWirelessDeviceRemoved(const QString& devicePath);

private:
    explicit DeviceManager(QObject* parent = nullptr);
    void init();

private slots:
    void processDeviceAdded(const QString& devicePath);
    void processDeviceRemoved(const QString& devicePath);
    void processDeviceStateChanged(NetworkManager::Device::State newstate,
                                   NetworkManager::Device::State oldstate);

private:
    static DeviceManager* instance_;
    // 当前接管的设备集合 QSet<DevicePath>
    QSet<QString> m_managedWiredDevices;
    QSet<QString> m_managedWirelessDevices;
};
}  // namespace Network
}  // namespace Kiran