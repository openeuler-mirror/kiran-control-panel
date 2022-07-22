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
#include "text-input-dialog.h"
#include "tray-itemwidget.h"

#define LIST_MAX_HEIGHT 358
#define TRAY_ITEM_NORAML_SIZE QSize(240, 50)
#define TRAY_ITEM_EXTENDED_SIZE QSize(240, 100)

ConnectionLists::ConnectionLists(QWidget* parent) : QListWidget(parent)
{
    qRegisterMetaType<NetworkConnectionInfo>("NetworkConnectionInfo");
    initUI();
    initConnect();
}

ConnectionLists::~ConnectionLists()
{
}

void ConnectionLists::initUI()
{
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setSelectionMode(SelectionMode::NoSelection);
    setFrameShape(QFrame::NoFrame);
    setContentsMargins(0, 0, 0, 0);
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
    NetworkConnectionInfo connectionInfo = item->data(Qt::UserRole).value<NetworkConnectionInfo>();
    QString activeConnectionPath = connectionInfo.activeConnectionPath;
    if (activeConnectionPath.isEmpty())
    {
        bool isWireless = connectionInfo.isWireless;
        QString connectionPath = connectionInfo.connectionPath;

        if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
        {
            // 若item的height为100,说明item已经展开，并进入了激活操作流程
            if (item->sizeHint().height() != 100)
            {
                item->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);

                for (int i = 0; i < this->count(); ++i)
                {
                    auto tmpItem = this->item(i);
                    if (tmpItem != item)
                    {
                        // activeConnectionPath 为空代表未激活
                        if (tmpItem->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
                        {
                            if (tmpItem->sizeHint().height() != 50)
                                itemSimpleStatus(tmpItem);
                        }
                    }
                }
                adjustTraySize();

                QWidget* widget = this->itemWidget(item);
                TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
                // Note:signalStrength == -1 对应连接隐藏网络
                if (connectionInfo.wirelessInfo.signalStrength == -1)
                    trayItemWidget->showInputSsidWidget();
                else
                    trayItemWidget->prepareConnectStatus();
            }
        }
        else
        {
            if (isWireless)
            {
                if (connectionInfo.wirelessInfo.signalStrength == -1)
                {
                    KLOG_DEBUG() << "plugin connectionInfo.wirelessInfo.signalStrength == -1";
                    TextInputDialog ssidInputDialog;
                    ssidInputDialog.setTitle(tr("Tips"));
                    QString tips = QString(tr("Please input a network name"));
                    ssidInputDialog.setText(tips);
                    connect(&ssidInputDialog, &TextInputDialog::ssid, this, &ConnectionLists::sendSsidToWireless);
                    ssidInputDialog.exec();
                }
                else
                    emit requestConnectWirelessNetwork(connectionInfo);
            }
            else
                emit requestActivateCurrentItemConnection(connectionPath);
        }
    }
    else
        KLOG_DEBUG() << "this connection is activated";
}

void ConnectionLists::showConnectionLists(ConnectionSettings::ConnectionType type)
{
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
        setFixedWidth(240);
    else
        this->setGridSize(QSize(240, 46));

    Device::Ptr device = findNetworkInterface(m_currentDevicePath);
    if (type == ConnectionSettings::Wired)
    {
        WiredDevice::Ptr wiredDevice = qobject_cast<WiredDevice*>(device);
        Connection::List availableConnections = wiredDevice->availableConnections();
        QString devicePath = wiredDevice->uni();
        KLOG_DEBUG() << "deviceName:" << wiredDevice->interfaceName();
        for (Connection::Ptr conn : availableConnections)
        {
            addConnectionToLists(conn, devicePath);
        }
    }
    else if (type == ConnectionSettings::Vpn)
    {
        // VPN的设备不明,VPN暂不指定设备
        Connection::List connectionList = listConnections();
        for (Connection::Ptr conn : connectionList)
        {
            if (conn->settings()->connectionType() == ConnectionSettings::Vpn)
                addConnectionToLists(conn, "");
        }
    }

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
    }
}

// TODO:优化代码
void ConnectionLists::addConnectionToLists(Connection::Ptr ptr, const QString& devicePath)
{
    if (ptr == nullptr)
    {
        KLOG_ERROR() << "ptr == null";
        return;
    }

    TrayItemWidget* trayItemWidget = nullptr;
    ConnectionItemWidget* connectionItemWidget = nullptr;
    QListWidgetItem* item = new QListWidgetItem();

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        trayItemWidget = new TrayItemWidget();
        trayItemWidget->setName(ptr->name());
        item->setSizeHint(TRAY_ITEM_NORAML_SIZE);
        adjustTraySize();
    }
    else
    {
        connectionItemWidget = new ConnectionItemWidget();
        connectionItemWidget->setName(ptr->name());
        item->setSizeHint(QSize(240, 36));
    }

    NetworkConnectionInfo connectionInfo;
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
            if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
            {
                connect(activeConnection.data(), &ActiveConnection::stateChanged, this, &ConnectionLists::handleActiveConnectionStateChanged, 
                Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
            }

            switch (activeConnection->state())
            {
            case ActiveConnection::Activating:
                if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                {
                    item->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);
                    adjustTraySize();
                    trayItemWidget->activatingStatus();
                }
                else
                {
                    connectionItemWidget->setLoadingStatus(true);
                    connectionItemWidget->setLabelVisible(true);
                }
                break;
            case ActiveConnection::Activated:
                if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                {
                    trayItemWidget->activatedStatus();
                    item->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);
                    adjustTraySize();
                }
                else
                    connectionItemWidget->activatedStatus();
                break;
            default:
                break;
            }
        }
    }
    connect(ptr.data(), &Connection::updated, this, &ConnectionLists::handleConnectionUpdated, Qt::UniqueConnection);
    this->addItem(item);

    QVariant var;
    var.setValue(connectionInfo);
    // item中保存connection的相关信息
    item->setData(Qt::UserRole, var);
    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        this->setItemWidget(item, connectionItemWidget);
        this->setMaximumHeight(this->gridSize().height() * this->count());
        m_itemWidgetMap.insert(connectionItemWidget, item);
        connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this, &ConnectionLists::handleEditButtonClicked);
    }
    else
    {
        this->setItemWidget(item, trayItemWidget);
        adjustTraySize();

        m_itemWidgetMap.insert(trayItemWidget, item);
        connect(trayItemWidget, &TrayItemWidget::disconnectButttonClicked, this, &ConnectionLists::handleDisconnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this, &ConnectionLists::handleConnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this, &ConnectionLists::handleCancelButtonClicked);
        trayItemWidget->setWiredStatusIcon();
    }

    updateGeometry();
}

void ConnectionLists::handleConnectionUpdated()
{
    auto ptr = qobject_cast<Connection*>(sender());
    emit connectionUpdated(ptr->path());
}

void ConnectionLists::handleActiveStateDeactivated(const QString& activatedConnectionPath)
{
    QListWidgetItem* activatedItem = findItemByActivatedPath(activatedConnectionPath);
    // 没有找到item则直接返回
    if (activatedItem == nullptr)
    {
        KLOG_DEBUG() << "Activated item was no found";
        return;
    }

    clearItemActivatedConnectionInfo(activatedItem);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
        enableConnectButtonOfItem(activatedItem, true);
    update();
}

void ConnectionLists::showWirelessNetworkLists()
{
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        setFixedWidth(240);
    }
    else
        this->setGridSize(QSize(240, 46));

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

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
    }
}

// TODO:优化代码
void ConnectionLists::addWirelessNetworkToLists(WirelessNetwork::Ptr network, const QString& devicePath)
{
    KLOG_DEBUG() << "network->ssid():" << network->ssid();
    AccessPoint::Ptr accessPoint = network->referenceAccessPoint();
    NetworkConnectionInfo connectionInfo;
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
    ConnectionSortListItem* item = new ConnectionSortListItem();

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        trayItemWidget = new TrayItemWidget();
        trayItemWidget->setName(network->ssid());
        trayItemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
        item->setSizeHint(TRAY_ITEM_NORAML_SIZE);
        adjustTraySize();
    }
    else
    {
        connectionItemWidget = new ConnectionItemWidget();
        connectionItemWidget->setName(network->ssid());
        connectionItemWidget->setEditButtonVisible(false);
        connectionItemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
        item->setSizeHint(QSize(240, 36));
    }

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
                if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                {
                    // 用于connection的状态通知
                    connect(activeConnection.data(), &ActiveConnection::stateChanged, this, &ConnectionLists::handleActiveConnectionStateChanged, 
                    Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));
                }
                switch (activeConnection->state())
                {
                case ActiveConnection::Activating:
                    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                    {
                        item->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);
                        adjustTraySize();
                        trayItemWidget->activatingStatus();
                    }
                    else
                    {
                        connectionItemWidget->setLoadingStatus(true);
                        connectionItemWidget->setLabelVisible(true);
                    }
                    break;
                case ActiveConnection::Activated:
                    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
                    {
                        item->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);
                        adjustTraySize();
                        trayItemWidget->activatedStatus();
                        trayItemWidget->setIgnoreButtonVisible(true);
                    }
                    else
                    {
                        connectionItemWidget->activatedStatus();
                        connectionItemWidget->setEditButtonVisible(true);
                    }
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

    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        this->setItemWidget(item, connectionItemWidget);
        this->setMaximumHeight(this->gridSize().height() * this->count());
        KLOG_DEBUG() << "this->gridSize().height():" << this->gridSize().height();
        KLOG_DEBUG() << "this->count():" << this->count();
        m_itemWidgetMap.insert(connectionItemWidget, item);
        connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this, &ConnectionLists::handleEditButtonClicked);
    }
    else
    {
        this->setItemWidget(item, trayItemWidget);

        adjustTraySize();

        m_itemWidgetMap.insert(trayItemWidget, item);
        connect(trayItemWidget, &TrayItemWidget::disconnectButttonClicked, this, &ConnectionLists::handleDisconnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this, &ConnectionLists::handleConnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this, &ConnectionLists::handleCancelButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::ignoreButtonClicked, this, &ConnectionLists::handleIgnoreButtonClicked);
    }
    // 根据signalStrength自定义排序
    this->sortItems();
}

void ConnectionLists::showOtherWirelessItem()
{
    TrayItemWidget* trayItemWidget = nullptr;
    ConnectionItemWidget* connectionItemWidget = nullptr;
    ConnectionSortListItem* item = new ConnectionSortListItem();

    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        trayItemWidget = new TrayItemWidget();
        trayItemWidget->setName(tr("Other WiFi networks"));
        trayItemWidget->setOtherNetworkIcon();
        item->setSizeHint(TRAY_ITEM_NORAML_SIZE);
        adjustTraySize();
    }
    else
    {
        connectionItemWidget = new ConnectionItemWidget();
        connectionItemWidget->setName(tr("Other WiFi networks"));
        connectionItemWidget->setEditButtonVisible(false);
        connectionItemWidget->setOtherNetworkIcon();
        item->setSizeHint(QSize(240, 36));
    }

    NetworkConnectionInfo connectionInfo;
    connectionInfo.isWireless = true;
    // 隐藏网络信号设为－１，以方便排序在最底层
    connectionInfo.wirelessInfo.signalStrength = -1;
    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
    this->addItem(item);

    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        this->setItemWidget(item, connectionItemWidget);
        this->setMaximumHeight(this->gridSize().height() * this->count());
        m_itemWidgetMap.insert(connectionItemWidget, item);
        connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this, &ConnectionLists::handleEditButtonClicked);
    }
    else
    {
        this->setItemWidget(item, trayItemWidget);
        m_itemWidgetMap.insert(trayItemWidget, item);
        connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this, &ConnectionLists::handleConnectButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this, &ConnectionLists::handleCancelButtonClicked);
        connect(trayItemWidget, &TrayItemWidget::sendSsid, this, &ConnectionLists::sendSsidToWireless);
    }
    this->sortItems();
}

void ConnectionLists::removeConnectionFromLists(const QString& path)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemPath = item->data(Qt::UserRole).value<NetworkConnectionInfo>().connectionPath;
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
    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
        this->setMaximumHeight(this->gridSize().height() * this->count());
    else
    {
        adjustTraySize();
    }
}

void ConnectionLists::removeWirelessNetworkFromLists(const QString& ssid)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemSsid = item->data(Qt::UserRole).value<NetworkConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            QWidget* itemWidget = this->itemWidget(item);
            itemWidget->deleteLater();
            this->takeItem(i);
            delete item;
            break;
        }
    }

    if (m_itemShowType == ITEM_WIDGET_TYPE_PLUGIN)
        this->setMaximumHeight(this->gridSize().height() * this->count());
    else
    {
        adjustTraySize();
    }
}

/**
  通过connectionLists->clear()释放所有的Item项
  但插入Item中的Widget并不会一起释放，需要另外释放
*/
void ConnectionLists::clearConnectionLists()
{
    int itemCount = this->count();
    for (int i = 0; i < itemCount; ++i)
    {
        QListWidgetItem* item = this->item(i);
        QWidget* itemWidget = this->itemWidget(item);
        itemWidget->deleteLater();
    }
    this->clear();
}

// Note:通过信号来激活加载动画
void ConnectionLists::updateItemActivatingStatus(QListWidgetItem* item)
{
    QWidget* widget = this->itemWidget(item);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->activatingStatus();
        item->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);
        adjustTraySize();
    }
    else
    {
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->setLoadingStatus(true);
        itemWidget->setLabelVisible(true);
    }
}

void ConnectionLists::updateItemActivatedStatus(const QString& activatedConnectionPath)
{
    auto activatedItem = findItemByActivatedPath(activatedConnectionPath);
    if (activatedItem == nullptr)
    {
        KLOG_DEBUG() << "activatedItem was no found";
        return;
    }
    QWidget* widget = this->itemWidget(activatedItem);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        activatedItem->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);
        adjustTraySize();
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->setLoadingStatus(false);
        trayItemWidget->activatedStatus();
        // xxx:待修改
        bool isWireless = activatedItem->data(Qt::UserRole).value<NetworkConnectionInfo>().isWireless;
        if (isWireless)
            trayItemWidget->setIgnoreButtonVisible(true);
    }
    else
    {
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->setLoadingStatus(false);
        itemWidget->activatedStatus();
        itemWidget->setEditButtonVisible(true);
    }
}

void ConnectionLists::updateItemActivatedPath(QListWidgetItem* item, QString activatedPath)
{
    NetworkConnectionInfo connectionInfo;
    connectionInfo = item->data(Qt::UserRole).value<NetworkConnectionInfo>();
    connectionInfo.activeConnectionPath = activatedPath;
    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
}

void ConnectionLists::clearItemActivatedConnectionInfo(QListWidgetItem* activatedItem)
{
    KLOG_DEBUG() << "clearItemActivatedConnectionInfo";
    QWidget* widget = this->itemWidget(activatedItem);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->deactivateStatus();
        activatedItem->setSizeHint(TRAY_ITEM_NORAML_SIZE);
        adjustTraySize();
    }
    else
    {
        ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
        itemWidget->deactivateStatus();
        bool isWireless = activatedItem->data(Qt::UserRole).value<NetworkConnectionInfo>().isWireless;
        if (isWireless)
        {
            itemWidget->setEditButtonVisible(false);
        }
    }
    // 赋值为空，清空已激活路径
    updateItemActivatedPath(activatedItem, "");
}

void ConnectionLists::showWiredStatusIcon()
{
    ConnectionItemWidget* connectionItemWidget;
    TrayItemWidget* trayItemWidget;
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        bool isWireless = item->data(Qt::UserRole).value<NetworkConnectionInfo>().isWireless;
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

void ConnectionLists::showInputPasswordWidgetOfItem(QListWidgetItem* item)
{
    item->setSizeHint(TRAY_ITEM_EXTENDED_SIZE);
    adjustTraySize();
    QWidget* widget = itemWidget(item);
    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
    trayItemWidget->showInputPasswordWidget();
    connect(trayItemWidget, &TrayItemWidget::sendPassword, this, &ConnectionLists::handleSendPassword);
}

void ConnectionLists::handleSendPassword(const QString& password)
{
    auto widget = qobject_cast<QWidget*>(sender());
    QListWidgetItem* item = m_itemWidgetMap.value(widget);
    int hiddenNetworkRow = this->count() - 1;
    QListWidgetItem* hiddenNetworkItem = this->item(hiddenNetworkRow);
    if (item == hiddenNetworkItem)
    {
        this->itemSimpleStatus(item);
    }
    emit sendPasswordToWirelessSetting(password);
}

QListWidgetItem* ConnectionLists::findItemByUuid(const QString& uuid)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemUuid = item->data(Qt::UserRole).value<NetworkConnectionInfo>().uuid;
        if (uuid == itemUuid)
        {
            return item;
        }
    }
    return nullptr;
}

QListWidgetItem* ConnectionLists::findItemBySsid(const QString& ssid)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString itemSsid = item->data(Qt::UserRole).value<NetworkConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            return item;
        }
    }
    return nullptr;
}

QListWidgetItem* ConnectionLists::findItemByActivatedPath(const QString& activatedPath)
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* item = this->item(i);
        QString activeConnectionPathFromItem = item->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath;
        if (activeConnectionPathFromItem == activatedPath)
        {
            return item;
        }
    }
    return nullptr;
}

void ConnectionLists::handleEditButtonClicked()
{
    auto connectionItemWidget = qobject_cast<ConnectionItemWidget*>(sender());
    auto item = m_itemWidgetMap.value(connectionItemWidget);
    QString uuid = item->data(Qt::UserRole).value<NetworkConnectionInfo>().uuid;
    QString activeConnectionPath = item->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath;
    bool isWireless = item->data(Qt::UserRole).value<NetworkConnectionInfo>().isWireless;
    KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
    if (isWireless)
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
    auto listItem = m_itemWidgetMap.value(trayItemWidget);
    QString activeConnectionPath = listItem->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath;
    emit trayRequestDisconnect(activeConnectionPath);
}

void ConnectionLists::handleConnectButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    auto item = m_itemWidgetMap.value(trayItemWidget);
    NetworkConnectionInfo connectionInfo = item->data(Qt::UserRole).value<NetworkConnectionInfo>();
    emit trayRequestConnect(connectionInfo);
}

void ConnectionLists::handleCancelButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    auto item = m_itemWidgetMap.value(trayItemWidget);
    trayItemWidget->simpleStatus();
    item->setSizeHint(TRAY_ITEM_NORAML_SIZE);
    adjustTraySize();

    updateGeometry();

    QString activeConnectionPathFromItem = item->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath;
    if (!activeConnectionPathFromItem.isEmpty())
    {
        auto activeConnectionsPathsLists = activeConnectionsPaths();
        for (QString path : activeConnectionsPathsLists)
        {
            if (activeConnectionPathFromItem == path)
            {
                ActiveConnection::Ptr activeConnection = findActiveConnection(path);
                if (activeConnection->state() == ActiveConnection::Activating)
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
    auto item = m_itemWidgetMap.value(trayItemWidget);
    trayItemWidget->simpleStatus();
    item->setSizeHint(TRAY_ITEM_NORAML_SIZE);
    adjustTraySize();

    item->data(Qt::UserRole).value<NetworkConnectionInfo>().connectionPath;
    QString activeConnectionPath = item->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath;
    emit trayRequestIgnore(activeConnectionPath);
}

void ConnectionLists::enableConnectButtonOfItem(QListWidgetItem* item, bool enable)
{
    QWidget* widget = this->itemWidget(item);
    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
    trayItemWidget->setEnableConnectButton(enable);
}

void ConnectionLists::itemSimpleStatus(QListWidgetItem* item)
{
    item->setSizeHint(TRAY_ITEM_NORAML_SIZE);
    QWidget* widget = this->itemWidget(item);
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(widget);
        trayItemWidget->simpleStatus();
    }
    adjustTraySize();
}

QListWidgetItem* ConnectionLists::getHiddenNetworkItem()
{
    int row = this->count() - 1;
    return item(row);
}

void ConnectionLists::handleActiveConnectionStateChanged(ActiveConnection::State state)
{
    auto activeConnection = qobject_cast<ActiveConnection*>(sender());
    QString id = "";
    if (activeConnection != nullptr)
        id = activeConnection->id();

    switch (state)
    {
    case ActiveConnection::State::Unknown:
        KLOG_DEBUG() << "ActiveConnection::State::Unknown";
        break;
    case ActiveConnection::State::Activating:
        KLOG_DEBUG() << "ActiveConnection::State::Activating";
        break;
    case ActiveConnection::State::Activated:
        KLOG_DEBUG() << "ActiveConnection::State::Activated";
        break;
    case ActiveConnection::State::Deactivating:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivating";
        break;
    case ActiveConnection::State::Deactivated:
        KLOG_DEBUG() << "ActiveConnection::State::Deactivated id:" << id;
        if (!id.isEmpty())
            StatusNotification::ActiveConnectionDeactivatedNotify(id);
        break;
    default:
        break;
    }
}

// TODO:优化adjustTraySize的变更策略
void ConnectionLists::adjustTraySize()
{
    int oldHeight = this->size().height();
    KLOG_DEBUG() << "oldHeight:" << oldHeight;
    // 需要更新页面大小
    int totalheight = 0;
    for (int i = 0; i < this->count(); i++)
    {
        totalheight += item(i)->sizeHint().height();
    }
    if (totalheight <= LIST_MAX_HEIGHT)
        setFixedHeight(totalheight);
    else
        setFixedHeight(LIST_MAX_HEIGHT);
    KLOG_DEBUG() << "this->size():" << this->size();
    KLOG_DEBUG() << "this->sizeHint():" << this->sizeHint();

    int newHeight = this->size().height();
    int changedHeight = newHeight - oldHeight;
    KLOG_DEBUG() << "changedHeight:" << changedHeight;
    QSize(this->sizeHint().width(), changedHeight);

    emit adjustedTraySize(QSize(this->sizeHint().width(), changedHeight));
}

void ConnectionLists::hideEvent(QHideEvent* event)
{
    if (m_itemShowType == ITEM_WIDGET_TYPE_TRAY)
    {
        for (int i = 0; i < this->count(); ++i)
        {
            auto tmpItem = this->item(i);
            // activeConnectionPath 为空代表未激活
            if (tmpItem->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
            {
                if (tmpItem->sizeHint().height() != 50)
                    itemSimpleStatus(tmpItem);
            }
        }
    }
    QWidget::hideEvent(event);
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
        if (!this->data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
            left = 1000;
        else
            left = this->data(Qt::UserRole).value<NetworkConnectionInfo>().wirelessInfo.signalStrength;

        if (!other.data(Qt::UserRole).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
            right = 1000;
        else
            right = other.data(Qt::UserRole).value<NetworkConnectionInfo>().wirelessInfo.signalStrength;

        return left > right;
    }
}
