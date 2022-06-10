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

#include "wireless-tray-widget.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Settings>
WirelessTrayWidget::WirelessTrayWidget(const QString &devicePath, QWidget *parent) : ConnectionTray(parent)
{
    m_devicePath = devicePath;
    init();
}

WirelessTrayWidget::~WirelessTrayWidget()
{
}
void WirelessTrayWidget::init()
{
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setSpacing(6);
    m_verticalLayout->setContentsMargins(0, 0, 0, 0);
    m_connectionLists = new ConnectionLists(this);
    m_verticalLayout->addWidget(m_connectionLists);

    Device::Ptr device = findNetworkInterface(m_devicePath);
    m_wirelessDevice = qobject_cast<WirelessDevice *>(device);
    initUI();
    initConnection();
}

void WirelessTrayWidget::initUI()
{
    m_connectionLists->setDevicePath(m_devicePath);
    m_connectionLists->setItemWidgetType(ITEM_WIDGET_TYPE_TRAY);
    m_connectionLists->showWirelessNetworkLists();
    m_connectionLists->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void WirelessTrayWidget::initConnection()
{
    connect(m_connectionLists, &ConnectionLists::trayRequestConnect,this, &WirelessTrayWidget::handleRequestConnectWirelessNetwork);
    connect(m_connectionLists, &ConnectionLists::trayRequestDisconnect, this, &WirelessTrayWidget::handleRequestDisconnect);

    connect(m_wirelessDevice.data(), &WirelessDevice::networkDisappeared, this, &WirelessTrayWidget::handleNetworkDisappeared);
    connect(m_wirelessDevice.data(), &WirelessDevice::networkAppeared, this, &WirelessTrayWidget::handleNetworkAppeared);

    connect(m_connectionLists, &ConnectionLists::sendPasswordToWirelessSetting, this, &WirelessTrayWidget::addAndActivateWirelessConnection);
}

void WirelessTrayWidget::handleRequestConnectWirelessNetwork(const ConnectionInfo &connectionInfo)
{
    m_connectionInfo = connectionInfo;
    QString ssid = connectionInfo.wirelessInfo.ssid;
    KLOG_DEBUG() << "handleRequestConnectWirelessNetwork ssid:" << ssid;
    QString accessPointPath = connectionInfo.wirelessInfo.accessPointPath;
    QString devicePath = connectionInfo.devicePath;

    getWirelessAvailableConnections(devicePath);

    if (m_wirelssConnectionMap.contains(ssid))
    {
        Connection::Ptr connection = m_wirelssConnectionMap.value(ssid);
        QString connectionPath = connection->path();
        KLOG_DEBUG() << "m_wirelssConnectionMap.value(ssid) connectionPath:" << connectionPath;
        activateWirelessConnection(connectionPath, devicePath, accessPointPath);
    }
    else
    {
        requireInputPassword(ssid);
        //        addAndActivateWirelessConnection(ssid, devicePath, accessPointPath);
    }
}

void WirelessTrayWidget::getWirelessAvailableConnections(const QString &devicePath)
{
    Device::Ptr device = findNetworkInterface(devicePath);
    Connection::List availableConnectionList = device->availableConnections();
    m_wirelssConnectionMap.clear();
    for (Connection::Ptr conn : availableConnectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::Wireless)
        {
            WirelessSetting::Ptr wirelessSetting = conn->settings()->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
            QString ssid = QString(wirelessSetting->ssid());
            KLOG_DEBUG() << "wirelessSetting->ssid():" << ssid;
            m_wirelssConnectionMap.insert(ssid, conn);
        }
    }
}

// 在已存在WirelessSetting配置的情况下，激活连接．（连接过一次后会创建WirelessSetting配置）
void WirelessTrayWidget::activateWirelessConnection(const QString &connectionPath, const QString &devicePath, const QString &accessPointPath)
{
    if (!connectionPath.isEmpty())
    {
        QDBusPendingReply<QDBusObjectPath> reply =
            NetworkManager::activateConnection(connectionPath, devicePath, accessPointPath);

        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_ERROR() << "activate connection failed:" << reply.error();
        }
        else
        {
            KLOG_DEBUG() << "reply.reply():" << reply.reply();
            QString activatedPath = reply.value().path();
        }
    }
}

// 不存在WirelessSetting配置的情况下，连接无线网络
void WirelessTrayWidget::addAndActivateWirelessConnection(const QString &password)
{
    const QString devicePath = m_connectionInfo.devicePath;
    const QString ssid = m_connectionInfo.wirelessInfo.ssid;
    const QString accessPointPath = m_connectionInfo.wirelessInfo.accessPointPath;

    Device::Ptr device = findNetworkInterface(devicePath);
    WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice *>(device);
    AccessPoint::Ptr accessPoint = wirelessDevice->findAccessPoint(accessPointPath);
    KLOG_DEBUG() << "accessPoint->ssid():" << accessPoint->ssid();

    QScopedPointer<ConnectionSettings> settings(new ConnectionSettings(ConnectionSettings::Wireless));
    settings->setId(ssid);
    QString uuid = settings->createNewUuid();
    settings->setUuid(uuid);

    WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wirelessSetting->setSsid(ssid.toUtf8());

    Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    // 处理不需要验证的情况
    AccessPoint::Capabilities capabilities = accessPoint->capabilities();
    AccessPoint::WpaFlags wpaFlags = accessPoint->wpaFlags();
    AccessPoint::WpaFlags rsnFlags = accessPoint->rsnFlags();

    WirelessSecuritySetting::KeyMgmt keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaNone;

    if (capabilities.testFlag(AccessPoint::Capability::Privacy) &&
        !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) &&
        !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
    {
        keyMgmt = WirelessSecuritySetting::KeyMgmt::Wep;
    }

    if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) ||
        rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk))
    {
        keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaPsk;
    }

    if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x) ||
        rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
    {
        keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaEap;
        //?
        //        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::Ieee8021x;
    }

    WirelessSecuritySetting::Ptr wirelessSecurity =
        settings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
    wirelessSecurity->setKeyMgmt(keyMgmt);

    if (keyMgmt != WirelessSecuritySetting::KeyMgmt::WpaNone)
    {
        setWirelessSecuritySetting(wirelessSecurity, password);
        // 暂不清楚效果
        //         wirelessSetting->setSecurity("802-11-wireless-security");
    }

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply =
        NetworkManager::addAndActivateConnection(settings->toMap(), wirelessDevice->uni(), accessPointPath);

    reply.waitForFinished();

    if (reply.isValid())
    {
    }
    else
    {
        KLOG_DEBUG() << "Connection failed: " << reply.error();
    }
}

void WirelessTrayWidget::setWirelessSecuritySetting(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting, const QString &psk)
{
    wirelessSecuritySetting->setPsk(psk);
    wirelessSecuritySetting->setPskFlags(Setting::SecretFlagType::None);  // default: Save password for all users
    wirelessSecuritySetting->setInitialized(true);
}

void WirelessTrayWidget::setWirelessSetting(const WirelessSetting::Ptr &wirelessSetting)
{
}

void WirelessTrayWidget::handleRequestDisconnect(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
}

void WirelessTrayWidget::handleActiveConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "handleActiveConnectionAdded :" << path;
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    QStringList deviceList = activatedConnection->devices();
    if ((activatedConnection->type() == ConnectionSettings::ConnectionType::Wireless) && deviceList.contains(m_devicePath))
    {
        ConnectionSettings::Ptr settings = activatedConnection->connection()->settings();
        WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        QString ssid = QString(wirelessSetting->ssid());
        m_connectionLists->findActiveItemBySsid(ssid);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WirelessTrayWidget::handleActiveConnectionStateChanged);
        // 加载等待动画
        m_connectionLists->connectionItemLoadingAnimation();
        m_connectionLists->enableConnectButtonOfActivatingItem(false);
    }
}

void WirelessTrayWidget::handleActiveConnectionRemoved(const QString &path)
{
}

void WirelessTrayWidget::handleStateActivated(const QString &activatedPath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedPath);
    QStringList deviceList = activeConnection->devices();

    if (deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wireless))
    {
        KLOG_DEBUG() << "handleStateActivated uuid :" << activeConnection->uuid();
        KLOG_DEBUG() << "handleStateActivated activatedPath:" << activatedPath;
        m_connectionLists->connectionStateNotify(ActiveConnection::Activated);
        KLOG_DEBUG() << "Wireless  ::connectionStateNotify";
        m_connectionLists->updateActivatedConnectionInfo(activatedPath);
        m_connectionLists->sortItems();
        m_connectionLists->update();
    }
}

void WirelessTrayWidget::handleStateDeactivated()
{
    m_connectionLists->connectionStateNotify(ActiveConnection::Deactivated);
    KLOG_DEBUG() << "handleStateDeactivated";
    m_connectionLists->clearDeactivatedConnectionInfo();
    m_connectionLists->enableConnectButtonOfActivatingItem(true);
    m_connectionLists->update();
}

void WirelessTrayWidget::handleNetworkDisappeared(const QString &ssid)
{
    KLOG_DEBUG() << "NetworkDisappeared ssid:" << ssid;
    m_connectionLists->removeWirelessNetworkFromLists(ssid);
}

void WirelessTrayWidget::handleNetworkAppeared(const QString &ssid)
{
    KLOG_DEBUG() << "NetworkAppeared ssid:" << ssid;
    WirelessNetwork::Ptr network = m_wirelessDevice->findNetwork(ssid);
    QString devicePath = m_wirelessDevice->uni();
    m_connectionLists->addWirelessNetworkToLists(network, devicePath);
}

void WirelessTrayWidget::requireInputPassword(const QString &ssid)
{
    int itemRow = m_connectionLists->findItemBySsid(ssid);
    m_connectionLists->showInputPasswordWidgetOfItem(itemRow);
}
