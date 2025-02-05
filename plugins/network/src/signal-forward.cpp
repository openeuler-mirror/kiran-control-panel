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

    m_Timer.setInterval(3000);
    m_Timer.setSingleShot(true);
    // Note:新设备插入后，需要等待一段时间，Device::List networkInterfaces() 来不及更新
    // Note:DeviceAdded signal is emitted when a new network interface is available.

    // XXX:当发出deviceAdded信号时，应该已经managed，需要修改并重新测试
    // deviceAdded信号发出时，根据信号的定义，此时device state为managed，但实际上并为unmanaged
    connect(notifier(), &Notifier::deviceAdded, this, &SignalForward::addDevice);
    connect(&m_Timer, &QTimer::timeout, this, [this]()
            {
                Device::Ptr device = findNetworkInterface(m_tmpDevicePath);
                if(!device.isNull() && device->managed())
                {
                    addDevice(m_tmpDevicePath);
                    m_Timer.stop();
                }
                else
                {
                    KLOG_INFO() << "this device interface is invalid!";
                    m_Timer.start();
                }
                m_waitCounts++;
                if(m_waitCounts > MAX_WAIT_COUNTS)
                {
                    KLOG_INFO() << "This device is currently invalid by NetworkManager";
                    m_Timer.stop();
                } });
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

//TODO:暂时使用uuid，之后统一使用path进行查询
//是否考虑可以直接传递Connection::Ptr
void SignalForward::editConnection(const NetworkConnectionInfo &connectionInfo)
{
    auto type = connectionInfo.type;
    switch (type)
    {
    case ConnectionSettings::ConnectionType::Wired:
        emit wiredConnectionEdited(connectionInfo.uuid,connectionInfo.activeConnectionPath);
        break;
    case ConnectionSettings::ConnectionType::Wireless:
        emit wirelessConnectionEdited(connectionInfo.uuid,connectionInfo.activeConnectionPath);
        break;
    case ConnectionSettings::ConnectionType::Vpn:
        emit vpnConnectionEdited();
        break;
    default:
        break;
    }
}

void SignalForward::addDevice(const QString &uni)
{
    Device::Ptr device = findNetworkInterface(uni);
    m_tmpDevicePath = uni;
    if (!device.isNull() && device->managed())
    {
        KLOG_INFO() << "add device:" << uni;
        switch (device->type())
        {
        case Device::Type::Ethernet:
            emit wiredDeviceAdded(uni);
            break;
        case Device::Type::Wifi:
            emit wirelessDeviceAdded(uni);
            break;
        default:
            emit otherDeviceAdded(uni);
            break;
        } 
    }
    else
    {
        KLOG_INFO() << "this device interface is invalid!";
        m_Timer.start();
        KLOG_INFO() << "wait device managed counts:" << m_waitCounts;
    }
}