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
#include <NetworkManagerQt/Utils>

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

    m_devicePtr = findNetworkInterface(m_devicePath);
    m_wirelessDevice = qobject_cast<WirelessDevice *>(m_devicePtr);
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
    connect(m_connectionLists, &ConnectionLists::trayRequestCancel, this, &WirelessTrayWidget::handleRequestCancel);
    connect(m_connectionLists, &ConnectionLists::trayRequestIgnore, this, &WirelessTrayWidget::handleRequestIgnore);

    connect(m_wirelessDevice.data(), &WirelessDevice::networkDisappeared, this, &WirelessTrayWidget::handleNetworkDisappeared);
    connect(m_wirelessDevice.data(), &WirelessDevice::networkAppeared, this, &WirelessTrayWidget::handleNetworkAppeared);

    connect(m_connectionLists, &ConnectionLists::sendPasswordToWirelessSetting, this, &WirelessTrayWidget::setSecurityPskAndActivateWirelessConnection);
    connect(m_connectionLists, &ConnectionLists::sendSsidToWirelessSetting, this, &WirelessTrayWidget::handleRequestConnectHiddenNetwork);

    connect(m_devicePtr.data(),&Device::stateChanged,this,&WirelessTrayWidget::handleDeviceStateChanged);
}

void WirelessTrayWidget::handleRequestConnectWirelessNetwork(const ConnectionInfo &connectionInfo)
{
    m_connectionInfo = connectionInfo;
    QString ssid = connectionInfo.wirelessInfo.ssid;
    QString accessPointPath = connectionInfo.wirelessInfo.accessPointPath;
    KLOG_DEBUG() << "handleRequestConnectWirelessNetwork ssid:" << ssid;

    getWirelessAvailableConnections(m_devicePath);
    if (m_wirelssConnectionMap.contains(ssid))
    {
        Connection::Ptr connection = m_wirelssConnectionMap.value(ssid);
        QString connectionPath = connection->path();
        activateWirelessConnection(connectionPath, m_devicePath, accessPointPath);
    }
    else
    {
        createConnectionSettings(ssid,accessPointPath);
        WirelessSecuritySetting::Ptr wirelessSecurity =
            m_connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
        WirelessSecuritySetting::KeyMgmt keyMgmt = wirelessSecurity->keyMgmt();
        if (keyMgmt != WirelessSecuritySetting::KeyMgmt::WpaNone)
            requireInputPassword(ssid);
        else
            addAndActivateWirelessConnection(m_connectionSettings);
    }
}

//默认需要密码，WpaPsk ，之后再改
void WirelessTrayWidget::handleRequestConnectHiddenNetwork(const QString &ssid)
{
    m_connectionInfo.wirelessInfo.ssid = ssid;
    //若要连接的隐藏网络已经被显式探测到了，则返回
    if(m_wirelessDevice->findNetwork(ssid) != nullptr)
    {
        return ;
    }
/** Note:连接隐藏网络时不指定AccessPointPath*/
    QString accessPointPath = "";
    getWirelessAvailableConnections(m_devicePath);
    if (m_wirelssConnectionMap.contains(ssid))
    {
        Connection::Ptr connection = m_wirelssConnectionMap.value(ssid);
        QString connectionPath = connection->path();
        activateWirelessConnection(connectionPath, m_devicePath, accessPointPath);
    }
    else
    {
        createConnectionSettings(ssid,accessPointPath);
        requireInputPassword(ssid);
    }
}

void WirelessTrayWidget::getWirelessAvailableConnections(const QString &devicePath)
{
    Connection::List availableConnectionList = m_devicePtr->availableConnections();
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
            KLOG_ERROR() << "activate connection failed:" << reply.error();
        else
            KLOG_DEBUG() << "reply.reply():" << reply.reply();
    }
}

// 不存在WirelessSetting配置，则根据AccessPoint创建配置
void WirelessTrayWidget::createConnectionSettings(const QString &ssid,const QString &accessPointPath)
{
    m_connectionSettings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wireless));
    m_connectionSettings->setId(ssid);
    m_connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

    WirelessSetting::Ptr wirelessSetting = m_connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wirelessSetting->setInitialized(true);
    wirelessSetting->setSsid(ssid.toUtf8());

    WirelessSecuritySetting::Ptr wirelessSecurity =
        m_connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
    wirelessSecurity->setInitialized(true);
    wirelessSetting->setSecurity(QStringLiteral("802-11-wireless-security"));

    Ipv4Setting::Ptr ipv4Setting = m_connectionSettings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    // 处理不同验证的情况
    //accessPointPath路径为空，对应隐藏网络情况，则默认为WpaPsk
    if(accessPointPath.isEmpty())
    {
        wirelessSetting->setHidden(true);
        WirelessSecuritySetting::KeyMgmt keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaPsk;
        wirelessSecurity->setKeyMgmt(keyMgmt);
    }
    else
    {
        AccessPoint::Ptr accessPoint = m_wirelessDevice->findAccessPoint(accessPointPath);
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
        }
        wirelessSecurity->setKeyMgmt(keyMgmt);
    }
}

void WirelessTrayWidget::addAndActivateWirelessConnection(ConnectionSettings::Ptr connectionSettings)
{
    KLOG_DEBUG() << "addAndActivateWirelessConnection";
    const QString ssid = m_connectionInfo.wirelessInfo.ssid;
    const QString accessPointPath = m_connectionInfo.wirelessInfo.accessPointPath;
    KLOG_DEBUG() << "accessPointPath" << accessPointPath;

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply =
        NetworkManager::addAndActivateConnection(connectionSettings->toMap(), m_devicePath, accessPointPath);

    reply.waitForFinished();
    if (reply.isError())
        KLOG_DEBUG() << "Connection failed: " << reply.error();
}

void WirelessTrayWidget::setWirelessSecurityPsk(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting, const QString &psk)
{
    wirelessSecuritySetting->setPsk(psk);
    wirelessSecuritySetting->setPskFlags(Setting::SecretFlagType::None);  // default: Save password for all users
    wirelessSecuritySetting->setInitialized(true);
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
        int row = m_connectionLists->findItemBySsid(ssid);
        if(row != -1)
        {
            //更新item信息
            m_connectionLists->setCurrentActiveItem(row);
            QListWidgetItem *activeItem = m_connectionLists->item(row);
            m_connectionLists->updateItemActivatedPath(activeItem, path);
        }
        else
        {
            row = m_connectionLists->count() - 1;
            m_connectionLists->itemSimpleStatus(row);
        }
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WirelessTrayWidget::handleActiveConnectionStateChanged,Qt::QueuedConnection);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, &m_statusNotification, &StatusNotification::connectionDeactivatedNotify,Qt::DirectConnection);
    }
}

void WirelessTrayWidget::handleActiveConnectionRemoved(const QString &path)
{
}

void WirelessTrayWidget::handleStateActivating(const QString &activatedPath)
{
    // 加载等待动画
    m_connectionLists->updateItemActivatingStatus();
}

void WirelessTrayWidget::handleStateActivated(const QString &activatedPath)
{
    KLOG_DEBUG() << "Wireless  handleStateActivated";
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedPath);
    if(activeConnection.isNull())
        return ;
    QStringList deviceList = activeConnection->devices();
    if (deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wireless))
    {
        KLOG_DEBUG() << "handleStateActivated activatedPath:" << activatedPath;
        m_connectionLists->updateItemActivatedStatus(activatedPath);

        int row = m_connectionLists->findItemByActivatedPath(activatedPath);
        ConnectionInfo connectionInfo = m_connectionLists->item(row)->data(Qt::UserRole).value<ConnectionInfo>();
        m_statusNotification.connectionStateNotify(ActiveConnection::Activated,connectionInfo);

        m_connectionLists->sortItems();
        m_connectionLists->update();
    }
}

void WirelessTrayWidget::handleStateDeactivated(const QString &activatedPath)
{
    KLOG_DEBUG() << "handleStateDeactivated activatedPath:" << activatedPath;
    m_connectionLists->handleActiveStateDeactivated(activatedPath);
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
    //没找到，对应隐藏网络
    if(itemRow == -1)
    {
        itemRow = m_connectionLists->count() - 1;
    }
    KLOG_DEBUG() << "itemRow:" << itemRow;
    m_connectionLists->showInputPasswordWidgetOfItem(itemRow);
}

//cancel暂时用disconnect代替
void WirelessTrayWidget::handleRequestCancel(const QString &activatedConnectionPath)
{
    KLOG_DEBUG() << "handleRequestCancel";
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
}

/**忽略该网络：断开连接并移除该网络配置*/
void WirelessTrayWidget::handleRequestIgnore(const QString &activatedConnectionPath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedConnectionPath);
    Connection::Ptr connection = activeConnection->connection();
    QSharedPointer<WirelessSetting> wirelessSetting = connection->settings()->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    QString ssid = QString(wirelessSetting->ssid());
    KLOG_DEBUG() << "Ignore ssid:" << ssid;
    m_wirelssConnectionMap.remove(ssid);

    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();

    QDBusPendingReply<> replyRemove = connection->remove();
    replyRemove.waitForFinished();
    if (replyRemove.isError())
        KLOG_INFO() << "Remove connection failed:" << reply.error();
}

void WirelessTrayWidget::setSecurityPskAndActivateWirelessConnection(const QString &password)
{
    WirelessSecuritySetting::Ptr wirelessSecurity =
        m_connectionSettings->setting(Setting::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();

    wirelessSecurity->setPsk(password);
    wirelessSecurity->setPskFlags(Setting::SecretFlagType::None);  // default: Save password for all users
    wirelessSecurity->setInitialized(true);
    addAndActivateWirelessConnection(m_connectionSettings);
}

void WirelessTrayWidget::handleDeviceStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason)
{
    //验证失败，删除配置，以便重新配置，避免出现重复的配置文件
    if((oldstate == Device::NeedAuth) && (newstate == Device::Failed))
    {
        QString ssid  = m_connectionInfo.wirelessInfo.ssid;
        Connection::List availableConnectionList = m_devicePtr->availableConnections();
        for (Connection::Ptr conn : availableConnectionList)
        {
            if (conn->settings()->connectionType() == ConnectionSettings::Wireless)
            {
                WirelessSetting::Ptr wirelessSetting = conn->settings()->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
                if(ssid == QString(wirelessSetting->ssid()))
                {
                    QDBusPendingReply<> replyRemove = conn->remove();
                    replyRemove.waitForFinished();
                    if (replyRemove.isError())
                        KLOG_INFO() << "Remove connection failed:" << replyRemove.error();
                }
            }
        }
    }
}
