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
}

ConnectionLists::~ConnectionLists()
{
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
    ConnectionInfo connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
    QString activeConnectionPath = connectionInfo.activeConnectionPath;
    if (activeConnectionPath.isEmpty())
    {
        bool isWireless = connectionInfo.isWireless;
        QString connectionPath = connectionInfo.connectionPath;
        if(m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
        {
            //若item的height为100,说明item已经展开，并进入了激活操作流程
            if(item->sizeHint().height() != 100)
            {
                item->setSizeHint(QSize(240,100));
                QWidget* widget = this->itemWidget(item);
                TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
                if(connectionInfo.wirelessInfo.signalStrength == -1)
                    trayItemWidget->showInputSsidWidget();
                else
                    trayItemWidget->prepareConnectStatus();
            }
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
    connectionInfo.id = ptr->name();
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
            connect(activeConnection.data(), &ActiveConnection::stateChanged, &m_statusNotification, &StatusNotification::connectionDeactivatedNotify,Qt::DirectConnection);
            //TODO:优化代码
            switch (activeConnection->state())
            {
            case ActiveConnection::Activating:
                if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                {
                    item->setSizeHint(QSize(240,100));
                    trayItemWidget->activatingStatus();
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
                {
                    trayItemWidget->activatedStatus();
                    item->setSizeHint(QSize(240, 100));
                }
                else
                    connectionItemWidget->activatedStatus();
                m_previousActivatedItem = item;
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
    QString path = activeConnection->path();
    switch (state)
    {
    case ActiveConnection::State::Deactivated:
        handleActiveStateDeactivated(path);
        break;
    default:
        break;
    }
}

void ConnectionLists::handleActiveStateDeactivated(const QString &activatedConnectionPath)
{
    KLOG_DEBUG() << "handleActiveStateDeactivated";
    int row = findItemByActivatedPath(activatedConnectionPath);
    //没有找到item则直接返回
    if(row == -1)
        return ;
    QListWidgetItem *activatedItem = item(row);
    clearItemActivatedConnectionInfo(activatedItem);
    enableConnectButtonOfItem(activatedItem,true);
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
    showOtherWirelessItem();
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
    item->setSizeHint(QSize(240, 50));

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
                connect(activeConnection.data(), &ActiveConnection::stateChanged, &m_statusNotification, &StatusNotification::connectionDeactivatedNotify,Qt::DirectConnection);
                switch (activeConnection->state())
                {
                case ActiveConnection::Activating:
                    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                    {
                        item->setSizeHint(QSize(240,100));
                        trayItemWidget->activatingStatus();
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
                    {
                        item->setSizeHint(QSize(240,100));
                        trayItemWidget->activatedStatus();
                        trayItemWidget->setIgnoreButtonVisible(true);
                    }
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
        connect(trayItemWidget, &TrayItemWidget::ignoreButtonClicked, this,&ConnectionLists::handleIgnoreButtonClicked);
    }

    // 根据signalStrength自定义排序
    this->sortItems();
}

void ConnectionLists::showOtherWirelessItem()
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
        connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this,&ConnectionLists::handleConnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this,&ConnectionLists::handleCancelButtonClicked);
        connect(trayItemWidget,&TrayItemWidget::sendSsid,this,&ConnectionLists::sendSsidToWirelessSetting);
    }
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
void ConnectionLists::updateItemActivatingStatus()
{
    m_currentActiveItem->setSizeHint(QSize(240,100));
    QWidget* widget = this->itemWidget(m_currentActiveItem);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->activatingStatus();
    }
    else
    {
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->setLoadingStatus(true);
        itemWidget->setLabelVisible(true);
    }
}

void ConnectionLists::updateItemActivatedStatus(const QString &activatedConnectionPath)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemActivedPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        if (activatedConnectionPath == itemActivedPath)
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

void ConnectionLists::clearItemActivatedConnectionInfo(QListWidgetItem *activatedItem)
{
    KLOG_DEBUG() << "clearItemActivatedConnectionInfo";
    activatedItem->setSizeHint(QSize(240,50));
    QWidget* widget = this->itemWidget(activatedItem);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->deactivateStatus();
    }
    else
    {
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->deactivateStatus();
        bool isWireless = activatedItem->data(Qt::UserRole).value<ConnectionInfo>().isWireless;
        if (isWireless)
        {
            itemWidget->setActionButtonVisible(false);
        }
    }
    // 赋值为空，清空已激活路径
    updateItemActivatedPath(activatedItem, "");
}

/*void ConnectionLists::connectionStateNotify(ActiveConnection::State state,const QString &activatedConnectionPath)
{
    KLOG_DEBUG() << "connectionStateNotify:" << activatedConnectionPath;

    QString summary, body, bodyStr, icon;
    ConnectionInfo connectionInfo;

    int row = findItemByActivatedPath(activatedConnectionPath);
    QListWidgetItem *activedItem = item(row);

    if (state == ActiveConnection::Activated)
    {
        connectionInfo = activedItem->data(Qt::UserRole).value<ConnectionInfo>();
        QWidget* widget = this->itemWidget(activedItem);

        summary = tr("Connection activated");
        body = tr("You are now connected to the network \"%1\"");
        if (connectionInfo.isWireless)
        {
            bodyStr = body.replace("the", "WIFI").arg(connectionInfo.wirelessInfo.ssid);
            int signal = connectionInfo.wirelessInfo.signalStrength;
            if (0 <= signal && signal < 25)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-1.svg";
            else if (25 <= signal && signal < 50)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-2.svg";
            else if (50 <= signal && signal < 75)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-3.svg";
            else if (75 <= signal && signal <= 100)
                icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-4.svg";
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
            icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wired-connection.svg";
        }
    }
    else if (state == ActiveConnection::Deactivated)
    {
        connectionInfo = activedItem->data(Qt::UserRole).value<ConnectionInfo>();
        QWidget* widget = this->itemWidget(activedItem);
        summary = tr("Connection deactivated");
        body = tr("You have now disconnected the network \"%1\"");
        if (connectionInfo.isWireless)
        {
            bodyStr = body.replace("the", "WIFI").arg(connectionInfo.wirelessInfo.ssid);
            icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wireless-error.svg";
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
            icon = "/home/lq/git/kiran-cpanel-network/resources/kcp-network-images/wired-disconnected.svg";
        }
    }

    NotifyNotification* notify = notify_notification_new(summary.toStdString().c_str(), bodyStr.toStdString().c_str(), icon.toStdString().c_str());
    notify_notification_show(notify, nullptr);
    g_object_unref(G_OBJECT(notify));
}*/

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
    item->setSizeHint(QSize(240,100));
    QWidget* widget = itemWidget(item);
    TrayItemWidget *trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
    trayItemWidget->showInputPasswordWidget();
    connect(trayItemWidget,&TrayItemWidget::sendPassword,this,&ConnectionLists::sendPasswordToWirelessSetting);
}


int ConnectionLists::findItemByUuid(const QString& uuid)
{
    // return -1 if no found
    int currentRow = -1;
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemUuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        if (uuid == itemUuid)
        {
            currentRow = this->row(item);
            break;
        }
    }
    return currentRow;
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

int ConnectionLists::findItemByActivatedPath(const QString& activatedPath)
{
    // return -1 if no found
    int currentRow = -1;
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString activeConnectionPathFromItem = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        if (activeConnectionPathFromItem == activatedPath)
        {
            currentRow = this->row(item);
            break;
        }
    }
    return currentRow;
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
    auto item =  m_itemWidgetMap.value(trayItemWidget);
    trayItemWidget->simpleStatus();
    item->setSizeHint(QSize(240,50));

    QString activeConnectionPathFromItem =  item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
    if(!activeConnectionPathFromItem.isEmpty())
    {
        auto activeConnectionsPathsLists =  activeConnectionsPaths();
        for (QString path : activeConnectionsPathsLists)
        {
            if(activeConnectionPathFromItem == path)
            {
                ActiveConnection::Ptr activeConnection = findActiveConnection(path);
                if(activeConnection->state() == ActiveConnection::Activating)
                {
                    emit trayRequestCancel(path);
                    KLOG_DEBUG() << "emit trayRequestCancel";
                }

            }
        }
    }
}

void ConnectionLists::handleIgnoreButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    auto item =  m_itemWidgetMap.value(trayItemWidget);
    trayItemWidget->simpleStatus();
    item->setSizeHint(QSize(240,50));

    item->data(Qt::UserRole).value<ConnectionInfo>().connectionPath;
    QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
    emit trayRequestIgnore(activeConnectionPath);
}

void ConnectionLists::enableConnectButtonOfItem(QListWidgetItem *item, bool enable)
{
    QWidget* widget = this->itemWidget(item);
    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
    trayItemWidget->setEnableConnectButton(enable);
}

void ConnectionLists::setCurrentActiveItem(int row)
{
    m_currentActiveItem = item(row);
}

void ConnectionLists::itemSimpleStatus(int row)
{
    auto item = this->item(row);
    item->setSizeHint(QSize(240,50));
    QWidget* widget = this->itemWidget(item);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->simpleStatus();
    }
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
