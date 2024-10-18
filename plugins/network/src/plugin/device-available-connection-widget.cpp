/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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

#include "device-available-connection-widget.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <style-property.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessSetting>
#include <QCollator>
#include <QLabel>
#include "connection-itemwidget.h"
#include "general.h"
#include "logging-category.h"
#include "signal-forward.h"
#include "utils.h"

using namespace NetworkManager;
using namespace NetworkUtils;

DeviceAvailableConnectionWidget::DeviceAvailableConnectionWidget(NetworkManager::Device::Ptr device, QWidget *parent)
    : KiranCollapse(parent),
      m_device(device)
{
    m_devicePath = m_device->uni();
    m_deviceType = m_device->type();
    KLOG_DEBUG(qLcNetwork) << m_device;

    initUI();
    initConnect();

    if (m_deviceType == Device::Ethernet)
    {
        m_wiredDevice = qobject_cast<WiredDevice *>(m_device);
        initWiredAvailableConnections();
        initWiredDeviceConnect();
    }
    else if (m_deviceType == Device::Wifi)
    {
        m_wirelessDevice = qobject_cast<WirelessDevice *>(m_device);
        initWirelessAppearNetwork();
        initWirelessDeviceConnect();
    }

    toggledSwitchButton(true);
}

DeviceAvailableConnectionWidget::~DeviceAvailableConnectionWidget()
{
}

void DeviceAvailableConnectionWidget::onAddConnection(Connection::Ptr connection)
{
    NetworkConnectionInfo connectionInfo;
    connectionInfo.id = connection->name();
    connectionInfo.uuid = connection->uuid();
    connectionInfo.connectionPath = connection->path();
    connectionInfo.devicePath = m_devicePath;
    connectionInfo.type = connection->settings()->connectionType();

    ConnectionItemWidget *connectionItemWidget = new ConnectionItemWidget(connectionInfo);
    connectionItemWidget->setName(connection->name());
    connectionItemWidget->setFixedHeight(PLUGIN_ITEM_WIDGET_HEIGHT);

    updateConnectionItemStatus(connectionItemWidget);
    addConnectionItem(connectionItemWidget);

    // this->sort();
}

void DeviceAvailableConnectionWidget::onAddWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network)
{
    AccessPoint::Ptr accessPoint = network->referenceAccessPoint();
    NetworkConnectionInfo connectionInfo;
    connectionInfo.devicePath = m_devicePath;
    connectionInfo.type = ConnectionSettings::Wireless;
    connectionInfo.isWireless = true;
    connectionInfo.wirelessInfo.ssid = network->ssid();
    connectionInfo.wirelessInfo.accessPointPath = accessPoint->uni();
    connectionInfo.wirelessInfo.signalStrength = accessPoint->signalStrength();
    KLOG_DEBUG(qLcNetwork) << "accessPoint signalStrength:" << connectionInfo.wirelessInfo.signalStrength;

    if (accessPoint->capabilities() == AccessPoint::Capability::None)
        connectionInfo.wirelessInfo.securitySetting = false;
    else
        connectionInfo.wirelessInfo.securitySetting = true;

    ConnectionItemWidget *connectionItemWidget = new ConnectionItemWidget(connectionInfo);
    connectionItemWidget->setName(network->ssid());
    connectionItemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
    connectionItemWidget->setFixedHeight(PLUGIN_ITEM_WIDGET_HEIGHT);

    updateConnectionItemStatus(connectionItemWidget);
    addConnectionItem(connectionItemWidget);
}

// NOTE:什么情况下activatedConnection会为空
// 无线网络连接不存在的隐藏网络时，有时会出现 findActiveConnection 为空的情况
void DeviceAvailableConnectionWidget::addActiveConnection(const QString &activePath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activePath);
    KLOG_DEBUG(qLcNetwork) << "add activatedConnection id:" << activatedConnection->id();
    QStringList deviceList = activatedConnection->devices();
    if (!deviceList.contains(m_devicePath))
    {
        return;
    }

    ConnectionItemWidget *activeItem;
    QString uuid = activatedConnection->uuid();
    KLOG_DEBUG(qLcNetwork) << "add activatedConnection uuid:" << uuid;
    activeItem = findConnectionItemByUuid(uuid);
    if (activeItem == nullptr)
    {
        ConnectionSettings::Ptr settings = activatedConnection->connection()->settings();
        WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        QString ssid = wirelessSetting->ssid();
        KLOG_DEBUG(qLcNetwork) << "add activatedConnection ssid:" << ssid;
        activeItem = findConnectionItemBySsid(ssid);
    }

    if (activeItem == nullptr)
    {
        KLOG_DEBUG(qLcNetwork) << "no found connection by uuid and ssid";
        return;
    }

    KLOG_DEBUG(qLcNetwork) << "Active Connection State:" << activatedConnection->state();
    activeItem->setActiveConnectionPath(activePath);
    activeItem->setActiveStatus(activatedConnection->state());

    m_switchButton->setChecked(true);

    // TODO:排序
    //  sort();

    connect(activatedConnection.data(), &ActiveConnection::stateChanged, activeItem, &ConnectionItemWidget::activeConnectionStateChanged, Qt::UniqueConnection);
}

// 断开网络时，会自动触发rescan搜索无线网络
void DeviceAvailableConnectionWidget::removeActiveConnection(const QString &activePath)
{
    ConnectionItemWidget *activeItem = findConnectionItemByActivePath(activePath);
    if (activeItem == nullptr)
    {
        return;
    }
    activeItem->activeConnectionStateChanged(ActiveConnection::State::Deactivated);
}

void DeviceAvailableConnectionWidget::addConnection(const QString &path)
{
    Connection::Ptr connection = findConnection(path);

    if (deviceType() == Device::Ethernet)
    {
        bool available = NetworkUtils::isAvailableConnection(m_devicePath, connection);
        KLOG_INFO(qLcNetwork) << m_device->type() << m_devicePath 
                              << "handle new connection:" << connection->name()
                              << "available:" << available;
        if (available)
        {
            onAddConnection(connection);
        }
    }
    else
    {
        KLOG_INFO(qLcNetwork) << m_device->type() << m_devicePath 
                              << "handle new connection:" << connection->name();
        onAddConnection(connection);
    }
}

void DeviceAvailableConnectionWidget::removeConnection(const QString &path)
{
    ConnectionItemWidget *connectionItem = findConnectionItemByPath(path);
    if (connectionItem == nullptr)
    {
        return;
    }
    removeConnectionItem(connectionItem);
    connectionItem->deleteLater();
}

void DeviceAvailableConnectionWidget::createConnection()
{
    emit SignalForward::instance()->createConnection(devicePath());
}

void DeviceAvailableConnectionWidget::updateConnection(const QString &connectionPath, const QString &mac)
{
    ConnectionItemWidget *connectionItem = findConnectionItemByPath(connectionPath);
    if (connectionItem == nullptr)
    {
        return;
    }

    // 判断配置是否应该出现在该设备下，应判断两个属性permanentHardwareAddress，hardwareAddress
    // 部分手机共享网络permanentHardwareAddress属性为空
    QString deviceMac = m_wiredDevice->permanentHardwareAddress();
    if (deviceMac.isEmpty())
    {
        deviceMac = m_wiredDevice->hardwareAddress();
    }

    if (deviceMac == mac)
    {
        auto connection = findConnection(connectionPath);
        onAddConnection(connection);
    }
    else
    {
        removeConnectionItem(connectionItem);
        connectionItem->deleteLater();
    }
}

void DeviceAvailableConnectionWidget::disappearNetwork(const QString &ssid)
{
    ConnectionItemWidget *connectionItem = findConnectionItemBySsid(ssid);
    if (connectionItem == nullptr)
    {
        return;
    }
    removeConnectionItem(connectionItem);
    connectionItem->deleteLater();
}

void DeviceAvailableConnectionWidget::appearNetwork(const QString &ssid)
{
    KLOG_DEBUG(qLcNetwork) << "appear network:" << ssid;
    WirelessNetwork::Ptr network = m_wirelessDevice->findNetwork(ssid);
    onAddWirelessNetwork(network);
}

void DeviceAvailableConnectionWidget::changeDeviceState(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason)
{
    if (newstate == Device::State::Activated)
    {
        auto activeConnection = m_device->activeConnection();
        m_activateLabel->setText(activeConnection->id());
    }
    else if (newstate == Device::Disconnected)
    {
        m_activateLabel->clear();
    }
}

void DeviceAvailableConnectionWidget::toggledSwitchButton(bool checked)
{
    if (!checked)
    {
        if (getIsExpand())
        {
            setCollapse();
        }
        for (auto connectionItem : m_connectionItemList)
        {
            connectionItem->disconnectConnection();
        }
    }

    if (checked && !getIsExpand())
    {
        setExpand();
    }
}

void DeviceAvailableConnectionWidget::initUI()
{
    m_contentWidget = new QWidget();
    m_contentWidgetLayout = new QVBoxLayout(m_contentWidget);
    m_contentWidgetLayout->setContentsMargins(0, 0, 0, 0);
    m_contentWidgetLayout->setSpacing(0);
    addExpansionSpaceWidget(m_contentWidget);
    setTobBarFixedHeight(36);

    setTitle(tr("Network card: %1").arg(m_device->interfaceName()));

    m_switchButton = new KiranSwitchButton(this);
    m_switchButton->setChecked(true);

    m_activateLabel = new QLabel();
    m_activateLabel->setStyleSheet("color:#919191;font-family: \"Noto Sans CJK SC Light\";");
    addTopBarWidget(m_activateLabel);
    addTopBarWidget(m_switchButton);

    if (m_deviceType == Device::Ethernet)
    {
        m_createConnectionButton = new QPushButton(m_contentWidget);
        Kiran::StylePropertyHelper::setButtonType(m_createConnectionButton, Kiran::BUTTON_Default);
        m_createConnectionButton->setIcon(QIcon(":/kcp-network-images/connection-add.svg"));
        m_contentWidgetLayout->addWidget(m_createConnectionButton);
        connect(m_createConnectionButton, &QPushButton::clicked, this, &DeviceAvailableConnectionWidget::createConnection);
    }
    else
    {
        addHiddenNetworkItem();
    }
}

void DeviceAvailableConnectionWidget::initConnect()
{
    connect(SignalForward::instance(), &SignalForward::activeConnectionRemoved, this, &DeviceAvailableConnectionWidget::removeActiveConnection);
    connect(SignalForward::instance(), &SignalForward::connectionRemoved, this, &DeviceAvailableConnectionWidget::removeConnection);

    connect(m_device.data(), &Device::stateChanged, this, &DeviceAvailableConnectionWidget::changeDeviceState, Qt::UniqueConnection);

    connect(m_switchButton, &KiranSwitchButton::toggled, this, &DeviceAvailableConnectionWidget::toggledSwitchButton);
}

void DeviceAvailableConnectionWidget::initWiredAvailableConnections()
{
    /**
     * NOTE:NetworkManager::availableConnections()是从org.freedesktop.NetworkManager中获取availableConnections属性。
     * NetworkManager源码中通过_nm_device_check_connection_available函数进行检查：
     * 1、如果连接配置中指定的MAC地址与当前设备的地址不一致，那么就不会出现在availableConnections列表中
     * 2、`an unrealized software device is always available, hardware devices never.`
     * 一个不可用的硬件设备是没有可用的连接。因此，但设备不可用时，仍要显示与设备相关的连接配置需要手动过滤处理。
     *
     */
    auto availableConnections = NetworkUtils::getAvailableWiredConnections(m_devicePath);
    for (auto connection : availableConnections)
    {
        onAddConnection(connection);
    }
}

void DeviceAvailableConnectionWidget::initWiredDeviceConnect()
{
    connect(SignalForward::instance(), &SignalForward::wiredActiveConnectionAdded, this, &DeviceAvailableConnectionWidget::addActiveConnection);
    connect(SignalForward::instance(), &SignalForward::wiredConnectionAdded, this, &DeviceAvailableConnectionWidget::addConnection);
    connect(SignalForward::instance(), &SignalForward::connectionMacChanged, this, &DeviceAvailableConnectionWidget::updateConnection);
}

void DeviceAvailableConnectionWidget::initWirelessAppearNetwork()
{
    WirelessNetwork::List wirelessNetworkList = m_wirelessDevice->networks();
    for (WirelessNetwork::Ptr network : wirelessNetworkList)
    {
        onAddWirelessNetwork(network);
    }
}

void DeviceAvailableConnectionWidget::initWirelessDeviceConnect()
{
    connect(SignalForward::instance(), &SignalForward::wirelessActiveConnectionAdded, this, &DeviceAvailableConnectionWidget::addActiveConnection);
    connect(m_wirelessDevice.data(), &WirelessDevice::networkAppeared, this, &DeviceAvailableConnectionWidget::appearNetwork);
    connect(m_wirelessDevice.data(), &WirelessDevice::networkDisappeared, this, &DeviceAvailableConnectionWidget::disappearNetwork);
}

void DeviceAvailableConnectionWidget::addConnectionItem(ConnectionItemWidget *newItem)
{
    // 排序
    int insertPosition = 0;
    if (deviceType() == Device::Wifi)
    {
        QList<int> list;
        int signalStrength = newItem->signalStrength();
        list << signalStrength;
        for (auto item : m_connectionItemList)
        {
            list << item->signalStrength();
        }
        // 从小到大
        std::sort(list.begin(), list.end());
        // 反转为从大到小
        std::reverse(list.begin(), list.end());
        insertPosition = list.indexOf(signalStrength);
    }
    else
    {
        QCollator collator;
        collator.setNumericMode(true);
        QStringList names;
        names << newItem->name();
        for (auto item : m_connectionItemList)
        {
            names << item->name();
        }
        std::sort(names.begin(), names.end(), collator);
        insertPosition = names.indexOf(newItem->name());
    }

    m_contentWidgetLayout->insertWidget(insertPosition, newItem);
    updateMaximumExpansionHeight();
    m_connectionItemList << newItem;
}

void DeviceAvailableConnectionWidget::removeConnectionItem(ConnectionItemWidget *item)
{
    m_contentWidgetLayout->removeWidget(item);
    m_connectionItemList.removeOne(item);
    updateMaximumExpansionHeight();
}

void DeviceAvailableConnectionWidget::addHiddenNetworkItem()
{
    NetworkConnectionInfo connectionInfo;
    connectionInfo.devicePath = m_devicePath;
    connectionInfo.type = ConnectionSettings::Wireless;
    connectionInfo.isWireless = true;
    // 隐藏网络信号设为－１，以方便排序在最底层
    connectionInfo.wirelessInfo.signalStrength = -1;

    ConnectionItemWidget *connectionItemWidget = new ConnectionItemWidget(connectionInfo);
    connectionItemWidget->setName(tr("Other WiFi networks"));
    connectionItemWidget->setEditButtonVisible(false);
    connectionItemWidget->setMoreOptionsVisible(false);
    connectionItemWidget->setOtherNetworkIcon();
    connectionItemWidget->setFixedHeight(PLUGIN_ITEM_WIDGET_HEIGHT);

    m_contentWidgetLayout->addWidget(connectionItemWidget);
    updateMaximumExpansionHeight();
}

void DeviceAvailableConnectionWidget::updateMaximumExpansionHeight()
{
    auto expansionMargin = KiranCollapse::expansionMargin();
    auto contentWidgetMargin = m_contentWidgetLayout->contentsMargins();
    auto contentWidgetSpacing = m_contentWidgetLayout->spacing();

    //计算KiranCollapse::expansionSpace最大展开高度
    int maximumHeight = expansionMargin.top() + expansionMargin.bottom() +
                        contentWidgetMargin.top() + contentWidgetMargin.bottom() +
                        (contentWidgetSpacing * (m_contentWidgetLayout->count() - 1)) +
                        (m_contentWidgetLayout->count() * PLUGIN_ITEM_WIDGET_HEIGHT);

    KiranCollapse::setMaximumExpansionHeight(maximumHeight);
}

// TODO:需要优化
void DeviceAvailableConnectionWidget::updateConnectionItemStatus(ConnectionItemWidget *item)
{
    NetworkConnectionInfo connectionInfo = item->connectionInfo();
    // 已连接的情况
    ActiveConnection::List activeConnectionList = activeConnections();
    for (ActiveConnection::Ptr activeConnection : activeConnectionList)
    {
        QStringList deviceList = activeConnection->devices();
        if (!deviceList.contains(m_devicePath))
        {
            continue;
        }

        if ((activeConnection->type() != ConnectionSettings::Wireless) &&
            (activeConnection->type() != ConnectionSettings::Wired))
        {
            continue;
        }

        if (connectionInfo.isWireless)
        {
            ConnectionSettings::Ptr settings = activeConnection->connection()->settings();
            WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
            QString ssid = QString(wirelessSetting->ssid());
            if (ssid == connectionInfo.wirelessInfo.ssid)
            {
                item->setActiveConnectionPath(activeConnection->path());
                item->setActiveStatus(activeConnection->state());
                m_activateLabel->setText(activeConnection->id());
                KLOG_DEBUG(qLcNetwork) << "current activeConnection state:" << activeConnection->state();
                connect(activeConnection.data(), &ActiveConnection::stateChanged, item, &ConnectionItemWidget::activeConnectionStateChanged, Qt::UniqueConnection);
            }
        }
        else if (activeConnection->uuid() == connectionInfo.uuid)
        {
            item->setActiveConnectionPath(activeConnection->path());
            item->setActiveStatus(activeConnection->state());
            m_activateLabel->setText(activeConnection->id());
            connect(activeConnection.data(), &ActiveConnection::stateChanged, item, &ConnectionItemWidget::activeConnectionStateChanged, Qt::UniqueConnection);
        }
    }
}

ConnectionItemWidget *DeviceAvailableConnectionWidget::findConnectionItemByUuid(const QString &uuid)
{
    for (auto item : m_connectionItemList)
    {
        if (item->uuid() == uuid)
        {
            return item;
        }
    }
    return nullptr;
}

ConnectionItemWidget *DeviceAvailableConnectionWidget::findConnectionItemByActivePath(const QString &activePath)
{
    for (auto item : m_connectionItemList)
    {
        if (item->activeConnectionPath() == activePath)
        {
            return item;
        }
    }
    return nullptr;
}

ConnectionItemWidget *DeviceAvailableConnectionWidget::findConnectionItemByPath(const QString &connectionPath)
{
    for (auto item : m_connectionItemList)
    {
        if (item->connectionPath() == connectionPath)
        {
            return item;
        }
    }
    return nullptr;
}

ConnectionItemWidget *DeviceAvailableConnectionWidget::findConnectionItemBySsid(const QString &ssid)
{
    for (auto item : m_connectionItemList)
    {
        if (item->ssid() == ssid)
        {
            return item;
        }
    }
    return nullptr;
}
