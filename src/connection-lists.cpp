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

#include "connection-lists.h"
#include <libnotify/notify.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <QPainter>
#include <QSvgRenderer>
#include "animation-loading-label.h"

ConnectionLists::ConnectionLists(QWidget* parent) : QListWidget(parent)
{
    initUI();
    initConnect();
    notify_init("ConnectionStateNotify");
}

ConnectionLists::~ConnectionLists()
{
    notify_uninit();
}

void ConnectionLists::initUI()
{
}

void ConnectionLists::initConnect()
{
    connect(this, &QListWidget::itemClicked, this, &ConnectionLists::handleConnectionItemClicked);
}

void ConnectionLists::setDevicePath(const QString& devicePath)
{
    m_currentDevicePath = devicePath;
}

void ConnectionLists::handleConnectionItemClicked(QListWidgetItem* item)
{
    // 判断是否已激活
    if (item != m_previousActivatedItem)
    {
        ConnectionInfo connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
        bool isWireless = connectionInfo.isWireless;
        QString connectionPath = connectionInfo.connectionPath;
        if (isWireless)
        {
            emit requestConnectWirelessNetwork(connectionInfo);
        }
        else
            emit requestActivateCurrentItemConnection(connectionPath);
    }
    else
        KLOG_DEBUG() << "this connection is activated";
}

void ConnectionLists::showConnectionLists(ConnectionSettings::ConnectionType type, ItemWidgetType itemType)
{
    m_itemShowType = itemType;
    if (type == ConnectionSettings::Wired)
    {
        Device::Ptr device = findNetworkInterface(m_currentDevicePath);
        WiredDevice::Ptr wiredDevice = qobject_cast<WiredDevice*>(device);

        Connection::List availableConnections = wiredDevice->availableConnections();
        QString devicePath = wiredDevice->uni();
        QString deviceName = wiredDevice->interfaceName();
        KLOG_DEBUG() << "devicePath:" << devicePath;
        KLOG_DEBUG() << "deviceName:" << deviceName;
        KLOG_DEBUG() << "availableConnections():" << availableConnections;
        for (Connection::Ptr conn : availableConnections)
        {
            addConnectionToLists(conn, devicePath);
        }
    }
    else if (type == ConnectionSettings::Vpn)
    {
        // vpn暂时不指定设备
        Connection::List connectionList = listConnections();
        for (Connection::Ptr conn : connectionList)
        {
            if (conn->settings()->connectionType() == ConnectionSettings::Vpn)
                addConnectionToLists(conn, "");
        }
    }
}

// VPN的设备不明,VPN暂不指定设备
void ConnectionLists::addConnectionToLists(Connection::Ptr ptr, const QString& devicePath)
{
    ConnectionItemWidget* itemWidget = new ConnectionItemWidget(m_itemShowType);
    itemWidget->setName(ptr->name());

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(200, 50));

    ConnectionInfo connectionInfo;
    connectionInfo.uuid = ptr->uuid();
    connectionInfo.connectionPath = ptr->path();
    connectionInfo.devicePath = devicePath;

    ActiveConnection::List activeConnectionList = activeConnections();
    for (ActiveConnection::Ptr activeConnection : activeConnectionList)
    {
        QStringList deviceList = activeConnection->devices();
        if (activeConnection->uuid() == ptr->uuid() && deviceList.contains(devicePath))
        {
            connectionInfo.activeConnectionPath = activeConnection->path();
            connect(activeConnection.data(), &ActiveConnection::stateChanged,this, &ConnectionLists::handleActiveConnectionStateChanged);
            itemWidget->activatedStatus();
            m_previousActivatedItem = item;
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    // item中保存connection的相关信息
    item->setData(Qt::UserRole, var);
    this->addItem(item);
    this->setItemWidget(item, itemWidget);
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));

    // button在插件页面执行编辑操作，在托盘页面执行断开连接操作
    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        connect(itemWidget, &ConnectionItemWidget::actionButtonClicked, [=]()
                {
                    QString uuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
                    QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
                    KLOG_DEBUG() << "uuid:" << uuid;
                    KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
                    emit requestEditConnection(uuid, activeConnectionPath);
                });
    }
    else
    {
        connect(itemWidget, &ConnectionItemWidget::actionButtonClicked, [=]()
                {
                    QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
                    emit trayRequestDisconnect(activeConnectionPath);
                });
        itemWidget->setWiredStatusIcon();
    }

    connect(ptr.data(), &Connection::updated, [=]()
            { emit connectionUpdated(ptr->path()); });
}

void ConnectionLists::handleActiveConnectionStateChanged(ActiveConnection::State state)
{
    auto activeConnection = qobject_cast<ActiveConnection*>(sender()) ;
    switch (state)
    {
    case ActiveConnection::State::Deactivated:
        handleActiveStateDeactivated();
        break;
    default:
        break;
    }
}

void ConnectionLists::handleActiveStateDeactivated()
{
    connectionStateNotify(ActiveConnection::Deactivated);
    clearDeactivatedConnectionInfo();
    update();
}

void ConnectionLists::showWirelessNetworkLists()
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == Device::Wifi)
        {
            KLOG_DEBUG() << "dev->interfaceName():" << dev->interfaceName();
            QSharedPointer<WirelessDevice> wirelessDevice = qobject_cast<WirelessDevice*>(dev);
            WirelessNetwork::List wirelessNetworkList = wirelessDevice->networks();
            QString devicePath = wirelessDevice->uni();
            for (WirelessNetwork::Ptr network : wirelessNetworkList)
            {
                addWirelessNetworkToLists(network, devicePath);
            }
        }
    }

    addOtherWirelessItemToLists();
}

void ConnectionLists::addWirelessNetworkToLists(WirelessNetwork::Ptr network, const QString& devicePath)
{
    KLOG_DEBUG() << "network->ssid():" << network->ssid();
    AccessPoint::Ptr accessPoint = network->referenceAccessPoint();
    ConnectionInfo connectionInfo;
    connectionInfo.isWireless = true;
    connectionInfo.wirelessInfo.ssid = network->ssid();
    connectionInfo.wirelessInfo.accessPointPath = accessPoint->uni();
    connectionInfo.wirelessInfo.signalStrength = accessPoint->signalStrength();
    connectionInfo.devicePath = devicePath;
    if (accessPoint->capabilities() == AccessPoint::Capability::None)
        connectionInfo.wirelessInfo.securitySetting = false;
    else
        connectionInfo.wirelessInfo.securitySetting = true;

    ConnectionItemWidget* itemWidget = new ConnectionItemWidget(ITEM_WIDGET_TYPE_TRAY);
    itemWidget->setName(network->ssid());
    itemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
    itemWidget->setActionButtonVisible(false);
    CustomSortListItem* item = new CustomSortListItem();
    item->setSizeHint(QSize(200, 50));

    // 已连接的情况
    ActiveConnection::List activeConnectionList = activeConnections();
    for (ActiveConnection::Ptr activeConnection : activeConnectionList)
    {
        if (activeConnection->type() == ConnectionSettings::Wireless)
        {
            ConnectionSettings::Ptr settings = activeConnection->connection()->settings();
            WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
            if (wirelessSetting->ssid() == connectionInfo.wirelessInfo.ssid)
            {
                connectionInfo.activeConnectionPath = activeConnection->path();
                connect(activeConnection.data(), &ActiveConnection::stateChanged, [=](ActiveConnection::State state)
                        {
                    switch (state)
                    {
                    case ActiveConnection::State::Deactivated:
                        KLOG_DEBUG() << "ActiveConnection::State::Deactivated";
                        //更新item所带信息，清空已激活路径
                        KLOG_DEBUG() << "Deactivated: activeConnection->path() : " << activeConnection->path();
                        emit deactivatedItemConnection(activeConnection->path());
                        break;
                    default:
                        break;
                    } });
                itemWidget->activatedStatus();
                itemWidget->setActionButtonVisible(true);
                m_previousActivatedItem = item;
            }
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
    this->addItem(item);
    this->setItemWidget(item, itemWidget);
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));

    connect(itemWidget, &ConnectionItemWidget::actionButtonClicked, [=]()
            {
        QString uuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
        if (!activeConnectionPath.isEmpty())
            emit requestEditConnection(uuid, activeConnectionPath);
        else
            KLOG_DEBUG() << "can not edit an unconnected wireless network "; });

    // 根据signalStrength自定义排序
    this->sortItems();
}

void ConnectionLists::addOtherWirelessItemToLists()
{
    ConnectionItemWidget* itemWidget = new ConnectionItemWidget(ITEM_WIDGET_TYPE_PLUGIN);
    itemWidget->setName(tr("Other WiFi networks"));
    CustomSortListItem* item = new CustomSortListItem();
    item->setSizeHint(QSize(200, 50));

    ConnectionInfo connectionInfo;
    // 隐藏网络信号设为－１，以方便排序在最底层
    connectionInfo.wirelessInfo.signalStrength = -1;
    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
    this->addItem(item);
    this->setItemWidget(item, itemWidget);
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));

    connect(itemWidget, &ConnectionItemWidget::actionButtonClicked, [=]()
            { emit requestCreatConnection(); });
    this->sortItems();
}

void ConnectionLists::removeConnectionFromLists(const QString& path)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemPath = item->data(Qt::UserRole).value<ConnectionInfo>().connectionPath;
        if (path == itemPath)
        {
            this->takeItem(i);
            // 使用takeItem:Items removed from a list widget will not be managed by Qt, and will need to be deleted manually.
            QWidget* itemWidget = this->itemWidget(item);
            itemWidget->deleteLater();
            delete item;
            break;
        }
    }
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));
}

void ConnectionLists::removeWirelessNetworkFromLists(const QString& ssid)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemSsid = item->data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            QWidget* itemWidget = this->itemWidget(item);
            itemWidget->deleteLater();
            this->takeItem(i);
            delete item;
            break;
        }
    }
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));
}

/**
  通过connectionLists->clear()释放所有的Item项
  但插入Item中的Widget并不会一起释放，需要另外释放
*/
void ConnectionLists::clearConnectionLists()
{
    KLOG_DEBUG() << "clearConnectionLists()";
    int itemCount = this->count();
    for (int i = 0; i < itemCount; ++i)
    {
        QListWidgetItem* item = this->item(i);
        QWidget* itemWidget = this->itemWidget(item);
        itemWidget->deleteLater();
    }
    this->clear();
    m_previousActivatedItem = nullptr;
}

void ConnectionLists::findItemByUuid(const QString& uuid)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemUuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        if (uuid == itemUuid)
        {
            m_currentActiveItem = item;
            break;
        }
    }
}

void ConnectionLists::findItemBySsid(const QString& ssid)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemSsid = item->data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            m_currentActiveItem = item;
            break;
        }
    }
}

// Note:通过信号来激活加载动画
void ConnectionLists::connectionItemLoadingAnimation()
{
    QWidget* widget = this->itemWidget(m_currentActiveItem);
    ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
    itemWidget->setLoadingStatus(true);
    itemWidget->setLabelVisible(true);
}

void ConnectionLists::updateActivatedConnectionInfo(QString activatedPath)
{
    m_previousActivatedItem = m_currentActiveItem;
    QWidget* widget = this->itemWidget(m_currentActiveItem);
    ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
    itemWidget->setLoadingStatus(false);
    itemWidget->activatedStatus();
    itemWidget->setActionButtonVisible(true);
    updateItemActivatedPath(m_currentActiveItem, activatedPath);
}

void ConnectionLists::updateItemActivatedPath(QListWidgetItem* item, QString activatedPath)
{
    ConnectionInfo connectionInfo;
    connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
    connectionInfo.activeConnectionPath = activatedPath;
    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
}

void ConnectionLists::clearDeactivatedConnectionInfo()
{
    KLOG_DEBUG() << "clearDeactivatedConnectionInfo";
    if (m_previousActivatedItem != nullptr)
    {
        QWidget* widget = this->itemWidget(m_previousActivatedItem);
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->deactivateStatus();
        bool isWireless = m_previousActivatedItem->data(Qt::UserRole).value<ConnectionInfo>().isWireless;
        if (isWireless)
        {
            itemWidget->setActionButtonVisible(false);
        }
        // 清空已激活路径,更新item所带信息
        updateItemActivatedPath(m_previousActivatedItem, "");
        m_previousActivatedItem = nullptr;
    }
    else
    {
        // 对应网络激活过程中激活失败，直接断开的情况
        QWidget* widget = this->itemWidget(m_currentActiveItem);
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->deactivateStatus();
    }
}

void ConnectionLists::connectionStateNotify(ActiveConnection::State state)
{
    QString summary, body, bodyStr, icon;
    ConnectionInfo connectionInfo;

    if (state == ActiveConnection::Activated)
    {
        connectionInfo = m_currentActiveItem->data(Qt::UserRole).value<ConnectionInfo>();
        QWidget* widget = this->itemWidget(m_currentActiveItem);
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);

        summary = tr("Connection activated");
        body = tr("You are now connected to the network \"%1\"");
        if (connectionInfo.isWireless)
        {
            bodyStr = body.replace("the", "WIFI").arg(connectionInfo.wirelessInfo.ssid);
            KLOG_DEBUG() << "bodyStr" << bodyStr;
            int signal = connectionInfo.wirelessInfo.signalStrength;
            if (0 <= signal && signal < 25)
                icon = "network-wireless-signal-weak-symbolic.symbolic";
            else if (25 <= signal && signal < 50)
                icon = "network-wireless-signal-ok-symbolic.symbolic";
            else if (50 <= signal && signal < 75)
                icon = "network-wireless-signal-good-symbolic.symbolic";
            else if (75 <= signal && signal <= 100)
                icon = "network-wireless-signal-excellent-symbolic.symbolic";
        }
        else
        {
            bodyStr = body.arg(itemWidget->getName());
            icon = "network-receive";
        }
    }
    else if (state == ActiveConnection::Deactivated)
    {
        ConnectionItemWidget* itemWidget;
        if (m_previousActivatedItem != nullptr)
        {
            connectionInfo = m_previousActivatedItem->data(Qt::UserRole).value<ConnectionInfo>();
            QWidget* widget = this->itemWidget(m_previousActivatedItem);
            itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        }
        else
        {
            // 对应网络激活过程中激活失败，直接断开的情况
            connectionInfo = m_currentActiveItem->data(Qt::UserRole).value<ConnectionInfo>();
            QWidget* widget = this->itemWidget(m_currentActiveItem);
            itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        }

        summary = tr("Connection deactivated");
        body = tr("You have now disconnected the network \"%1\"");
        if (connectionInfo.isWireless)
        {
            bodyStr = body.replace("the", "WIFI").arg(connectionInfo.wirelessInfo.ssid);
            KLOG_DEBUG() << "bodyStr" << bodyStr;
            icon = "network-wireless-offline-symbolic.symbolic";
        }
        else
        {
            bodyStr = body.arg(itemWidget->getName());
            icon = "network-offline";
        }
    }

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
}

void ConnectionLists::showWiredStatusIcon()
{
    ConnectionItemWidget* itemWidget;
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        bool isWireless = item->data(Qt::UserRole).value<ConnectionInfo>().isWireless;
        if (!isWireless)
        {
            QWidget* widget = this->itemWidget(item);
            itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
            itemWidget->setWiredStatusIcon();
        }
    }
}


// void ConnectionLists::topActivatedItem(int row)
//{
//         QListWidgetItem* topItem = takeItem(row);
//         insertItem(0,topItem);
// }

CustomSortListItem::CustomSortListItem(QWidget* parent)
{
}

bool CustomSortListItem::operator<(const QListWidgetItem& other) const
{
    if (this->data(Qt::UserRole).isNull() || other.data(Qt::UserRole).isNull())
    {
        return QListWidgetItem::operator<(other);
    }
    else
    {
        int left = this->data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.signalStrength;
        int right = other.data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.signalStrength;
        return left > right;
    }
}
