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
#include "tray-itemwidget.h"

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
    setFixedWidth(240);
}

void ConnectionLists::initConnect()
{
    connect(this, &QListWidget::itemClicked, this, &ConnectionLists::handleConnectionItemClicked);
}

void ConnectionLists::setDevicePath(const QString& devicePath)
{
    m_currentDevicePath = devicePath;
}

void ConnectionLists::setItemWidgetType(ItemWidgetType itemType)
{
    m_itemShowType = itemType;
}

void ConnectionLists::handleConnectionItemClicked(QListWidgetItem* item)
{
    // 判断是否已激活
    if (item != m_previousActivatedItem)
    {
        ConnectionInfo connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
        bool isWireless = connectionInfo.isWireless;
        QString connectionPath = connectionInfo.connectionPath;
        if(m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
        {
            item->setSizeHint(QSize(240,100));
            QWidget* widget = this->itemWidget(item);
            TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
            trayItemWidget->prepareConnectStatus();
        }
        else
        {
            if (isWireless)
                emit requestConnectWirelessNetwork(connectionInfo);
            else
                emit requestActivateCurrentItemConnection(connectionPath);
        }
    }
    else
        KLOG_DEBUG() << "this connection is activated";
}

void ConnectionLists::showConnectionLists(ConnectionSettings::ConnectionType type)
{
    if (type == ConnectionSettings::Wired)
    {
        Device::Ptr device = findNetworkInterface(m_currentDevicePath);
        WiredDevice::Ptr wiredDevice = qobject_cast<WiredDevice*>(device);

        Connection::List availableConnections = wiredDevice->availableConnections();
        QString devicePath = wiredDevice->uni();
        QString deviceName = wiredDevice->interfaceName();
        KLOG_DEBUG() << "devicePath:" << devicePath;
        KLOG_DEBUG() << "deviceName:" << deviceName;
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
    TrayItemWidget* trayItemWidget = nullptr;
    ConnectionItemWidget* connectionItemWidget = nullptr;

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        trayItemWidget = new TrayItemWidget();
        trayItemWidget->setName(ptr->name());
    }
    else
    {
        connectionItemWidget = new ConnectionItemWidget();
        connectionItemWidget->setName(ptr->name());
    }

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(240, 50));

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
            connect(activeConnection.data(), &ActiveConnection::stateChanged, this, &ConnectionLists::handleActiveStateChanged);
            switch (activeConnection->state())
            {
            case ActiveConnection::Activating:
                if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                {
                    trayItemWidget->activatingStatus();
                }
                else
                {
                    connectionItemWidget->setLoadingStatus(true);
                    connectionItemWidget->setLabelVisible(true);
                }
                m_currentActiveItem = item;
                item->setSizeHint(QSize(240,100));
                break;
            case ActiveConnection::Activated:
                if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                    trayItemWidget->activatedStatus();
                else
                    connectionItemWidget->activatedStatus();
                m_previousActivatedItem = item;
                item->setSizeHint(QSize(240, 100));
                break;
            default:
                break;
            }

        }
    }
    connect(ptr.data(), &Connection::updated, this, &ConnectionLists::handleConnectionUpdated);
    this->addItem(item);
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));

    QVariant var;
    var.setValue(connectionInfo);
    // item中保存connection的相关信息
    //TODO:将connectionInfo存到Widget的Property中
    item->setData(Qt::UserRole, var);
    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        this->setItemWidget(item, connectionItemWidget);
        m_itemWidgetMap.insert(connectionItemWidget,item);
        connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this,&ConnectionLists::handleEditButtonClicked);
    }
    else
    {
        this->setItemWidget(item, trayItemWidget);
        m_itemWidgetMap.insert(trayItemWidget,item);
        connect(trayItemWidget, &TrayItemWidget::disconnectButttonClicked, this,&ConnectionLists::handleDisconnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this,&ConnectionLists::handleConnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this,&ConnectionLists::handleCancelButtonClicked);
        trayItemWidget->setWiredStatusIcon();
    }
}

void ConnectionLists::handleConnectionUpdated()
{
    auto ptr = qobject_cast<Connection*>(sender());
    emit connectionUpdated(ptr->path());
}

void ConnectionLists::handleActiveStateChanged(ActiveConnection::State state)
{
    auto activeConnection = qobject_cast<ActiveConnection*>(sender());
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
    Device::Ptr device = findNetworkInterface(m_currentDevicePath);
    if (device->type() == Device::Wifi)
    {
        KLOG_DEBUG() << "dev->interfaceName():" << device->interfaceName();
        QSharedPointer<WirelessDevice> wirelessDevice = qobject_cast<WirelessDevice*>(device);
        WirelessNetwork::List wirelessNetworkList = wirelessDevice->networks();
        QString devicePath = wirelessDevice->uni();
        for (WirelessNetwork::Ptr network : wirelessNetworkList)
        {
            addWirelessNetworkToLists(network, devicePath);
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
    KLOG_DEBUG() << "accessPoint->signalStrength():" << connectionInfo.wirelessInfo.signalStrength;
    connectionInfo.devicePath = devicePath;
    if (accessPoint->capabilities() == AccessPoint::Capability::None)
        connectionInfo.wirelessInfo.securitySetting = false;
    else
        connectionInfo.wirelessInfo.securitySetting = true;

    TrayItemWidget* trayItemWidget = nullptr;
    ConnectionItemWidget* connectionItemWidget = nullptr;

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        trayItemWidget = new TrayItemWidget();
        trayItemWidget->setName(network->ssid());
        trayItemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
    }
    else
    {
        connectionItemWidget = new ConnectionItemWidget();
        connectionItemWidget->setName(network->ssid());
        connectionItemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
    }
    ConnectionSortListItem* item = new ConnectionSortListItem();
    item->setSizeHint(QSize(220, 50));

    // 已连接的情况
    ActiveConnection::List activeConnectionList = activeConnections();
    for (ActiveConnection::Ptr activeConnection : activeConnectionList)
    {
        QStringList deviceList = activeConnection->devices();
        if (activeConnection->type() == ConnectionSettings::Wireless)
        {
            ConnectionSettings::Ptr settings = activeConnection->connection()->settings();
            WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
            QString ssid = QString(wirelessSetting->ssid());
            if ((ssid == connectionInfo.wirelessInfo.ssid) && deviceList.contains(devicePath))
            {
                connectionInfo.activeConnectionPath = activeConnection->path();
                connect(activeConnection.data(), &ActiveConnection::stateChanged, this, &ConnectionLists::handleActiveStateChanged);
                switch (activeConnection->state())
                {
                case ActiveConnection::Activating:
                    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                    {
                        trayItemWidget->activatedStatus();
                    }
                    else
                    {
                        connectionItemWidget->setLoadingStatus(true);
                        connectionItemWidget->setLabelVisible(true);
                    }
                    m_currentActiveItem = item;
                    break;
                case ActiveConnection::Activated:
                    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                        trayItemWidget->activatedStatus();
                    else
                        connectionItemWidget->activatedStatus();
                    m_previousActivatedItem = item;
                    break;
                default:
                    break;
                }
            }
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
    this->addItem(item);
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));

    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        this->setItemWidget(item, connectionItemWidget);
        m_itemWidgetMap.insert(connectionItemWidget,item);
        connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this,&ConnectionLists::handleEditButtonClicked);
    }
    else
    {
        this->setItemWidget(item, trayItemWidget);
        m_itemWidgetMap.insert(trayItemWidget,item);
        connect(trayItemWidget, &TrayItemWidget::disconnectButttonClicked, this,&ConnectionLists::handleDisconnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this,&ConnectionLists::handleConnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this,&ConnectionLists::handleCancelButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::ignoreButtonClicked, [=](){
                    emit trayRequestIgnore("");
                });
    }

    // 根据signalStrength自定义排序
    this->sortItems();
}

void ConnectionLists::addOtherWirelessItemToLists()
{
    TrayItemWidget* trayItemWidget = nullptr;
    ConnectionItemWidget* connectionItemWidget = nullptr;

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        trayItemWidget = new TrayItemWidget();
        trayItemWidget->setName(tr("Other WiFi networks"));
        trayItemWidget->setOtherNetworkIcon();
    }
    else
    {
        connectionItemWidget = new ConnectionItemWidget();
        connectionItemWidget->setName(tr("Other WiFi networks"));
        connectionItemWidget->setActionButtonVisible(false);
        connectionItemWidget->setOtherNetworkIcon();
    }

    ConnectionSortListItem* item = new ConnectionSortListItem();
    item->setSizeHint(QSize(200, 50));

    ConnectionInfo connectionInfo;
    // 隐藏网络信号设为－１，以方便排序在最底层
    connectionInfo.wirelessInfo.signalStrength = -1;
    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
    this->addItem(item);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
        this->setItemWidget(item, trayItemWidget);
    else
        this->setItemWidget(item, connectionItemWidget);
    this->setMaximumHeight(this->sizeHintForRow(0) * this->count() + (2 * this->frameWidth()));

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
            m_itemWidgetMap.remove(itemWidget);
            KLOG_DEBUG() << "m_itemWidgetMap.remove(itemWidget):" << m_itemWidgetMap.remove(itemWidget);
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

// Note:通过信号来激活加载动画
void ConnectionLists::connectionItemLoadingAnimation()
{
    QWidget* widget = this->itemWidget(m_currentActiveItem);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->setLoadingStatus(true);
//        trayItemWidget->setLoadingLabelVisible(true);
    }
    else
    {
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->setLoadingStatus(true);
        itemWidget->setLabelVisible(true);
    }
}

void ConnectionLists::updateActivatedConnectionInfo(QString activatedPath)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemActivedPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        if (activatedPath == itemActivedPath)
        {
            m_currentActiveItem = item;
            break;
        }
    }

    m_previousActivatedItem = m_currentActiveItem;
    m_currentActiveItem->setSizeHint(QSize(240, 100));
    QWidget* widget = this->itemWidget(m_currentActiveItem);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->setLoadingStatus(false);
        trayItemWidget->activatedStatus();
        //xxx:待修改
        bool isWireless = m_currentActiveItem->data(Qt::UserRole).value<ConnectionInfo>().isWireless;
        if (isWireless)
            trayItemWidget->setIgnoreButtonVisible(true);
    }
    else
    {
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->setLoadingStatus(false);
        itemWidget->activatedStatus();
        itemWidget->setActionButtonVisible(true);
    }
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
        m_previousActivatedItem->setSizeHint(QSize(240,50));
        QWidget* widget = this->itemWidget(m_previousActivatedItem);
        if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
        {
            TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
            trayItemWidget->deactivateStatus();
            trayItemWidget->setLoadingStatus(false);
        }
        else
        {
            ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
            itemWidget->deactivateStatus();
            bool isWireless = m_previousActivatedItem->data(Qt::UserRole).value<ConnectionInfo>().isWireless;
            if (isWireless)
            {
                itemWidget->setActionButtonVisible(false);
            }
        }
        // 清空已激活路径,更新item所带信息
        updateItemActivatedPath(m_previousActivatedItem, "");
        m_previousActivatedItem = nullptr;
    }
    else
    {
        // 对应网络激活过程中激活失败，直接断开的情况
        QWidget* widget;
        if (m_currentActiveItem != nullptr)
        {
            m_currentActiveItem->setSizeHint(QSize(240,50));
            widget = this->itemWidget(m_currentActiveItem);
            if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
            {
                TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
                trayItemWidget->deactivateStatus();
            }
            else
            {
                ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
                itemWidget->deactivateStatus();
            }
        }
        else
        {
            KLOG_DEBUG() << "-------------";
        }
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
            if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
            {
                TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
                bodyStr = body.arg(trayItemWidget->getName());
            }
            else
            {
                ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
                bodyStr = body.arg(itemWidget->getName());
            }
            icon = "network-receive";
        }
    }
    else if (state == ActiveConnection::Deactivated)
    {
        QWidget* widget;
        if (m_previousActivatedItem != nullptr)
        {
            connectionInfo = m_previousActivatedItem->data(Qt::UserRole).value<ConnectionInfo>();
            widget = this->itemWidget(m_previousActivatedItem);
        }
        else
        {
            // 对应网络激活过程中激活失败，直接断开的情况
            connectionInfo = m_currentActiveItem->data(Qt::UserRole).value<ConnectionInfo>();
            widget = this->itemWidget(m_currentActiveItem);
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
            if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
            {
                TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
                bodyStr = body.arg(trayItemWidget->getName());
            }
            else
            {
                ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
                bodyStr = body.arg(itemWidget->getName());
            }
            icon = "network-offline";
        }
    }

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
}

void ConnectionLists::showWiredStatusIcon()
{
    ConnectionItemWidget* connectionItemWidget;
    TrayItemWidget* trayItemWidget;
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        bool isWireless = item->data(Qt::UserRole).value<ConnectionInfo>().isWireless;
        if (!isWireless)
        {
            QWidget* widget = this->itemWidget(item);
            if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
            {
                trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
                trayItemWidget->setWiredStatusIcon();
            }
            else
            {
                connectionItemWidget = qobject_cast<ConnectionItemWidget*>(widget);
                connectionItemWidget->setWiredStatusIcon();
            }
        }
    }
}

void ConnectionLists::showInputPasswordWidgetOfItem(int itemRow)
{
    auto item = this->item(itemRow);
    QWidget* widget = itemWidget(item);
    TrayItemWidget *trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
    trayItemWidget->showInputPasswordWidget();
    connect(trayItemWidget,&TrayItemWidget::sendPassword,this,&ConnectionLists::sendPasswordToWirelessSetting);
}

//xxx:待修改，不要直接赋值给m_currentActiveItem，返回QListWidgetItem*,使其更加通用
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

void ConnectionLists::findActiveItemBySsid(const QString& ssid)
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

int ConnectionLists::findItemBySsid(const QString& ssid)
{
    // return -1 if no found
    int currentRow = -1;
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemSsid = item->data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            currentRow = this->row(item);
            break;
        }
    }
    return currentRow;
}

int ConnectionLists::getPasswordWidgetRow()
{
    return m_passwordWidgetRow;
}

void ConnectionLists::handleEditButtonClicked()
{
    auto connectionItemWidget = qobject_cast<ConnectionItemWidget*>(sender());
    auto item =  m_itemWidgetMap.value(connectionItemWidget);
    QString uuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
    QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
    bool isWireless = item->data(Qt::UserRole).value<ConnectionInfo>().isWireless;
    KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
    if(isWireless)
    {
        if (!activeConnectionPath.isEmpty())
            emit requestEditConnection(uuid, activeConnectionPath);
        else
            KLOG_DEBUG() << "can not edit an unconnected wireless network ";
    }
    else
        emit requestEditConnection(uuid, activeConnectionPath);

}

void ConnectionLists::handleDisconnectButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    auto listItem =  m_itemWidgetMap.value(trayItemWidget);
    QString activeConnectionPath = listItem->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
    emit trayRequestDisconnect(activeConnectionPath);
}

void ConnectionLists::handleConnectButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    auto item =  m_itemWidgetMap.value(trayItemWidget);
    ConnectionInfo connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
    emit trayRequestConnect(connectionInfo);
}

void ConnectionLists::handleCancelButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    auto listItem =  m_itemWidgetMap.value(trayItemWidget);
    trayItemWidget->simpleStatus();
    listItem->setSizeHint(QSize(240,50));
    emit trayRequestCancel();
}

void ConnectionLists::handleIgnoreButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
}

void ConnectionLists::enableConnectButtonOfActivatingItem(bool enable)
{
    QWidget* widget = this->itemWidget(m_currentActiveItem);
    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
    trayItemWidget->setEnableConnectButton(enable);
}

ConnectionSortListItem::ConnectionSortListItem(QWidget* parent)
{
}

bool ConnectionSortListItem::operator<(const QListWidgetItem& other) const
{
    if (this->data(Qt::UserRole).isNull() || other.data(Qt::UserRole).isNull())
    {
        return QListWidgetItem::operator<(other);
    }
    else
    {
        int left, right;
        if (!this->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath.isEmpty())
            left = 1000;
        else
            left = this->data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.signalStrength;

        if (!other.data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath.isEmpty())
            right = 1000;
        else
            right = other.data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.signalStrength;

        return left > right;
    }
}
