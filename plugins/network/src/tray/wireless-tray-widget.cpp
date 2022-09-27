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
#include "general.h"
#include "signal-forward.h"
#include "status-notification.h"
using namespace NetworkManager;

WirelessTrayWidget::WirelessTrayWidget(const QString &devicePath, QWidget *parent) : TrayWidget(parent),
                                                                                     m_connectionList(nullptr)
{
    m_devicePath = devicePath;
    m_devicePtr = findNetworkInterface(m_devicePath);
    m_wirelessDevice = qobject_cast<WirelessDevice *>(m_devicePtr);
    init();
}

WirelessTrayWidget::~WirelessTrayWidget()
{
}

void WirelessTrayWidget::init()
{
    initUI();
    initConnection();
    ActiveConnection::Ptr activatedConnection = m_wirelessDevice->activeConnection();
    if (!activatedConnection.isNull())
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WirelessTrayWidget::handleActiveConnectionStateChanged, Qt::UniqueConnection);
}

void WirelessTrayWidget::initUI()
{
    m_connectionList = new TrayConnectionList(this);
    addWidget(m_connectionList);

    m_connectionList->setDevicePath(m_devicePath);
    m_connectionList->showWirelessNetworkList();
}

void WirelessTrayWidget::initConnection()
{
    connect(m_connectionList, &TrayConnectionList::activateSelectedWirelessNetwork, this, &WirelessTrayWidget::handleActivateSelectedWirelessNetwork);
    connect(m_connectionList, &TrayConnectionList::disconnect, this, &WirelessTrayWidget::handleDisconnect);
    connect(m_connectionList, &TrayConnectionList::cancelConnection, this, &WirelessTrayWidget::handleCancelConnection);
    connect(m_connectionList, &TrayConnectionList::ignoreConnection, this, &WirelessTrayWidget::handleIgnoreConnection);

    connect(m_wirelessDevice.data(), &WirelessDevice::networkDisappeared, this, &WirelessTrayWidget::handleNetworkDisappeared);
    connect(m_wirelessDevice.data(), &WirelessDevice::networkAppeared, this, &WirelessTrayWidget::handleNetworkAppeared);

    connect(m_connectionList, &TrayConnectionList::sendPasswordToWirelessSetting, this, &WirelessTrayWidget::setSecurityPskAndActivateWirelessConnection);
    connect(m_connectionList, &TrayConnectionList::sendSsidToWireless, this, &WirelessTrayWidget::handleActivateHiddenNetwork);

    connect(m_devicePtr.data(), &Device::stateChanged, this, &WirelessTrayWidget::handleDeviceStateChanged, Qt::UniqueConnection);
    connect(m_connectionList, &TrayConnectionList::sizeChanged, this, &WirelessTrayWidget::sizeChanged);

    connect(SignalForward::instance(), &SignalForward::wirelessConnectionAdded, this, &WirelessTrayWidget::handleNotifierConnectionAdded);
    connect(SignalForward::instance(), &SignalForward::wirelessActiveConnectionAdded, this, &WirelessTrayWidget::handleActiveConnectionAdded);
}

void WirelessTrayWidget::handleActivateSelectedWirelessNetwork(const NetworkConnectionInfo &connectionInfo)
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
        createConnectionSettings(ssid, accessPointPath);
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
void WirelessTrayWidget::handleActivateHiddenNetwork(const QString &ssid)
{
    m_connectionInfo.wirelessInfo.ssid = ssid;
    //若要连接的隐藏网络已经被显式探测到了，则返回
    if (m_wirelessDevice->findNetwork(ssid) != nullptr)
    {
        KLOG_DEBUG() << "Hidden networks have been explicitly detected,return";
        StatusNotification::connectitonHiddenNetworkFailedNotify(ssid);
        // 将排在最后的隐藏网络item复原
        int row = m_connectionList->count() - 1;
        auto *hiddenNetworkItemWidget = m_connectionList->itemWidget(row);
        m_connectionList->setItemWidgetSimpleStatus(hiddenNetworkItemWidget);
        return;
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
        createConnectionSettings(ssid, accessPointPath);
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
// Note:不存在的无线网络或者配置，能activate成功，但是返回的ActivateConnection可能会空，从而导致错误
void WirelessTrayWidget::activateWirelessConnection(const QString &connectionPath, const QString &devicePath, const QString &accessPointPath)
{
    if (!connectionPath.isEmpty())
    {
        KLOG_DEBUG() << " wireless device path" << devicePath;
        QDBusPendingReply<QDBusObjectPath> reply =
            NetworkManager::activateConnection(connectionPath, devicePath, accessPointPath);

        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_ERROR() << "activate connection failed:" << reply.error();
            StatusNotification::connectitonFailedNotify(connectionPath);
        }
        else
            KLOG_DEBUG() << "reply.reply():" << reply.reply();
    }
}

// 不存在WirelessSetting配置，则根据AccessPoint创建配置
void WirelessTrayWidget::createConnectionSettings(const QString &ssid, const QString &accessPointPath)
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
    // accessPointPath路径为空，对应隐藏网络情况，则默认为WpaPsk
    if (accessPointPath.isEmpty())
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

// Note:不存在的无线网络或者配置，activate成功，但是返回的ActivateConnection可能会空，从而导致错误
void WirelessTrayWidget::addAndActivateWirelessConnection(ConnectionSettings::Ptr connectionSettings)
{
    const QString ssid = m_connectionInfo.wirelessInfo.ssid;
    const QString accessPointPath = m_connectionInfo.wirelessInfo.accessPointPath;
    KLOG_DEBUG() << "accessPointPath" << accessPointPath;
    KLOG_DEBUG() << "wireless device path:" << m_devicePath;
    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply =
        NetworkManager::addAndActivateConnection(connectionSettings->toMap(), m_devicePath, accessPointPath);

    reply.waitForFinished();
    if (reply.isError())
    {
        StatusNotification::connectitonFailedNotifyByName(ssid);
        KLOG_DEBUG() << "Connection failed: " << reply.error();
    }
}

void WirelessTrayWidget::setWirelessSecurityPsk(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting, const QString &psk)
{
    wirelessSecuritySetting->setPsk(psk);
    wirelessSecuritySetting->setPskFlags(Setting::SecretFlagType::None);  // default: Save password for all users
    wirelessSecuritySetting->setInitialized(true);
}

void WirelessTrayWidget::handleDisconnect(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
}

/*FIX:
 *由于ActiveConnection为空，暂时无法获得有效的连接信息
 *存在多个网卡时，由于无法确定是那个该ActiveConnection来自那个设备，则多个设备都会发送通知
 */
void WirelessTrayWidget::handleActiveConnectionAdded(const QString &path)
{
    //多个网卡，还需要判断设备
    KLOG_DEBUG() << "handleActiveConnectionAdded :" << path;
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection == nullptr)
    {
        // Note:目前已知连接一个不存在的无线网络时，activatedConnection为空
        StatusNotification::connectitonFailedNotify();
        KLOG_DEBUG() << "new add activatedConnection is nullptr";
        return;
    }

    QStringList deviceList = activatedConnection->devices();
    if (deviceList.contains(m_devicePath))
    {
        ConnectionSettings::Ptr settings = activatedConnection->connection()->settings();
        WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        QString ssid = QString(wirelessSetting->ssid());

        auto *activeItemWidget = m_connectionList->findItemWidgetBySsid(ssid);
        if (activeItemWidget != nullptr)
        {
            //更新item信息
            m_connectionList->updateItemWidgetActivePath(activeItemWidget, path);
        }
        else
        {
            //将排在最后的隐藏网络item复原
            int row = m_connectionList->count() - 1;
            auto *hiddenNetworkItemWidget = m_connectionList->itemWidget(row);
            m_connectionList->setItemWidgetSimpleStatus(hiddenNetworkItemWidget);
        }
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WirelessTrayWidget::handleActiveConnectionStateChanged, Qt::UniqueConnection);
    }
}

void WirelessTrayWidget::handleActiveConnectionRemoved(const QString &path)
{
    KLOG_DEBUG() << "ConnectionRemoved";
    m_connectionList->handleActiveStateDeactivated(path);
}

void WirelessTrayWidget::handleStateActivating(const QString &activePath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activePath);
    if (activatedConnection == nullptr)
    {
        // Note:目前已知连接一个不存在的无线网络时，activatedConnection为空
        StatusNotification::connectitonFailedNotify();
        KLOG_DEBUG() << "activatedConnection == nullptr";
        return;
    }
    QStringList deviceList = activatedConnection->devices();
    if ((activatedConnection->type() == ConnectionSettings::ConnectionType::Wireless) && deviceList.contains(m_devicePath))
    {
        m_connectionList->setItemWidgetStatus(activePath, ActiveConnection::State::Activating);
    }

    QDBusPendingReply<> replyRequestScan = m_wirelessDevice->requestScan();
    replyRequestScan.waitForFinished();
    KLOG_DEBUG() << "State Activating requestScan";
    if (replyRequestScan.isError())
    {
        KLOG_DEBUG() << "State Activating requestScan error:" << replyRequestScan.error();
    }
    else
    {
        KLOG_DEBUG() << "State Activating requestScan reply:" << replyRequestScan.reply();
    }
}

void WirelessTrayWidget::handleStateActivated(const QString &activePath)
{
    KLOG_DEBUG() << "Wireless State: Activated";
    ActiveConnection::Ptr activeConnection = findActiveConnection(activePath);
    if (activeConnection.isNull())
        return;
    QStringList deviceList = activeConnection->devices();
    if (deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wireless))
    {
        KLOG_DEBUG() << "handleStateActivated activePath:" << activePath;
        m_connectionList->setItemWidgetStatus(activePath, ActiveConnection::State::Activated);

        auto itemWidget = m_connectionList->findItemWidgetByActivePath(activePath);
        NetworkConnectionInfo connectionInfo = itemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>();
        StatusNotification::ActiveConnectionActivatedNotify(connectionInfo);
        m_connectionList->sort();
        m_connectionList->update();

        //连接成功后手动rescan
        QDBusPendingReply<> replyRequestScan = m_wirelessDevice->requestScan();
        replyRequestScan.waitForFinished();
        if (replyRequestScan.isError())
        {
            KLOG_DEBUG() << "StateActivated requestScan error:" << replyRequestScan.error();
        }
        else
        {
            KLOG_DEBUG() << "StateActivated requestScan reply:" << replyRequestScan.reply();
        }
    }
}

void WirelessTrayWidget::handleNotifierConnectionAdded(const QString &path)
{
}

void WirelessTrayWidget::handleNotifierConnectionRemoved(const QString &path)
{
}

void WirelessTrayWidget::handleStateDeactivated(const QString &activatedPath)
{
    KLOG_DEBUG() << "StateDeactivated :" << activatedPath;
    m_connectionList->handleActiveStateDeactivated(activatedPath);
}

void WirelessTrayWidget::handleNetworkDisappeared(const QString &ssid)
{
    KLOG_DEBUG() << "NetworkDisappeared ssid:" << ssid;
    m_connectionList->removeWirelessNetworkFromList(ssid);
    m_connectionList->adjustTraySize();
}

void WirelessTrayWidget::handleNetworkAppeared(const QString &ssid)
{
    KLOG_DEBUG() << "NetworkAppeared ssid:" << ssid;
    WirelessNetwork::Ptr network = m_wirelessDevice->findNetwork(ssid);
    QString devicePath = m_wirelessDevice->uni();
    m_connectionList->addWirelessNetwork(network, devicePath);
}

void WirelessTrayWidget::requireInputPassword(const QString &ssid)
{
    auto *itemWidget = m_connectionList->findItemWidgetBySsid(ssid);
    //没找到，对应隐藏网络
    if (itemWidget == nullptr)
    {
        int row = m_connectionList->count() - 1;
        itemWidget = m_connectionList->itemWidget(row);
    }
    m_connectionList->showPasswordEdit(itemWidget);
}

// cancel暂时用disconnect代替
void WirelessTrayWidget::handleCancelConnection(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
    else
        KLOG_INFO() << "deactivate Connection:" << reply.reply();
}

/**忽略该网络：断开连接并移除该网络配置*/
void WirelessTrayWidget::handleIgnoreConnection(const QString &activatedConnectionPath)
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

    /*
     * Note:deactivate后，通过信号发出deactivate的状态通知，通知需要从connection中获取id信息
     * 如果deactivate后立马删除connection,可能导致状态通知获取不到相应的信息
     * 故延时一段时间，以便状态通知能获取到id信息
     * 有待改进
     * */

    QTimer::singleShot(100, this, [connection]()
                       {
                           QDBusPendingReply<> replyRemove = connection->remove();
                           replyRemove.waitForFinished();
                           if (replyRemove.isError())
                               KLOG_INFO() << "Remove connection failed:" << replyRemove.error(); });
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
    KLOG_DEBUG() << "Device::StateChangeReason reason" << reason;
    //验证失败，删除配置，以便重新配置，避免出现重复的配置文件
    if ((oldstate == Device::NeedAuth) && (newstate == Device::Failed))
    {
        QString ssid = m_connectionInfo.wirelessInfo.ssid;
        Connection::List availableConnectionList = m_devicePtr->availableConnections();
        for (Connection::Ptr conn : availableConnectionList)
        {
            if (conn->settings()->connectionType() == ConnectionSettings::Wireless)
            {
                WirelessSetting::Ptr wirelessSetting = conn->settings()->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
                if (ssid == QString(wirelessSetting->ssid()))
                {
                    QDBusPendingReply<> replyRemove = conn->remove();
                    replyRemove.waitForFinished();
                    if (replyRemove.isError())
                        KLOG_INFO() << "Remove connection failed:" << replyRemove.error();
                }
            }
        }
        // TODO:弹窗 要求重新输入密码
    }

    if (reason == NetworkManager::Device::SsidNotFound)
    {
        KLOG_DEBUG() << "NetworkManager::Device::SsidNotFound";
        QString body, bodyStr;
        body = tr("the network \"%1\" not found");
        bodyStr = body.arg(m_connectionInfo.wirelessInfo.ssid);
        // StatusNotification::connectitonFailedNotifyByReason(bodyStr);
    }
}
