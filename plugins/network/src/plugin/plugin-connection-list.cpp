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

#include "plugin-connection-list.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessSetting>
#include "connection-itemwidget.h"
#include "general.h"
#include "text-input-dialog.h"

using namespace NetworkManager;
#define PLUGIN_ITEM_WIDGET_HEIGHT 36

PluginConnectionList::PluginConnectionList(QWidget* parent) : ConnectionList(parent)
{
    initUI();
    initConnect();
}

PluginConnectionList::~PluginConnectionList()
{
}

void PluginConnectionList::initUI()
{
    setWidgetContentsSpacing(10);
    setWidgetContentsMargins(0, 0, 0, 10);
}

void PluginConnectionList::initConnect()
{
}

void PluginConnectionList::addConnection(NetworkManager::Connection::Ptr ptr, const QString& devicePath)
{
    if (ptr == nullptr)
    {
        KLOG_ERROR() << "ptr == null";
        return;
    }

    // TODO:确定new ConnectionItemWidget() 的parentWidget


    NetworkConnectionInfo connectionInfo;
    connectionInfo.id = ptr->name();
    connectionInfo.uuid = ptr->uuid();
    connectionInfo.connectionPath = ptr->path();
    connectionInfo.devicePath = devicePath;
    connectionInfo.type = ptr->settings()->connectionType();

    ConnectionItemWidget* connectionItemWidget = new ConnectionItemWidget(connectionInfo);
    connectionItemWidget->setName(ptr->name());
    connectionItemWidget->setFixedHeight(PLUGIN_ITEM_WIDGET_HEIGHT);

    ActiveConnection::List activeConnectionList = activeConnections();
    for (ActiveConnection::Ptr activeConnection : activeConnectionList)
    {
        QStringList deviceList = activeConnection->devices();
        if (activeConnection->uuid() == ptr->uuid() &&
            (deviceList.contains(devicePath) || devicePath.isEmpty()))
        {
            connectionInfo.activeConnectionPath = activeConnection->path();
            switch (activeConnection->state())
            {
            case ActiveConnection::Activating:
                connectionItemWidget->setLoadingStatus(true);
                connectionItemWidget->setLabelVisible(true);
                break;
            case ActiveConnection::Activated:
                connectionItemWidget->activatedStatus();
                break;
            default:
                break;
            }
        }
    }

    connect(ptr.data(), &Connection::updated, this, &PluginConnectionList::handleConnectionUpdated, Qt::UniqueConnection);

    QVariant var;
    var.setValue(connectionInfo);
    connectionItemWidget->setProperty(PROPERTY_NETWORK_CONNECTION_INFO, var);
    connectionItemWidget->setAccessibleName(QString("WiredConnectionItem::%1").arg(connectionInfo.id));

    this->addWidget(connectionItemWidget);
    this->setMaximumHeight((this->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * this->count());
//    this->setFixedHeight((this->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * this->count());
    //根据名称自定义排序
    this->sort();

    connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this, &PluginConnectionList::handleEditButtonClicked);
    connect(connectionItemWidget, &ConnectionItemWidget::clicked, this, &PluginConnectionList::handleConnectionItemClicked);
}

void PluginConnectionList::addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network,
                                              const QString& devicePath)
{
    KLOG_DEBUG() << "network ssid to be added:" << network->ssid();
    AccessPoint::Ptr accessPoint = network->referenceAccessPoint();
    NetworkConnectionInfo connectionInfo;
    connectionInfo.isWireless = true;
    connectionInfo.wirelessInfo.ssid = network->ssid();
    connectionInfo.wirelessInfo.accessPointPath = accessPoint->uni();
    connectionInfo.wirelessInfo.signalStrength = accessPoint->signalStrength();
    KLOG_DEBUG() << "accessPoint signalStrength:" << connectionInfo.wirelessInfo.signalStrength;
    connectionInfo.devicePath = devicePath;
    if (accessPoint->capabilities() == AccessPoint::Capability::None)
        connectionInfo.wirelessInfo.securitySetting = false;
    else
        connectionInfo.wirelessInfo.securitySetting = true;

    ConnectionItemWidget* connectionItemWidget = new ConnectionItemWidget();
    connectionItemWidget->setName(network->ssid());
    connectionItemWidget->setEditButtonVisible(false);
    connectionItemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);

    connectionItemWidget->setFixedHeight(PLUGIN_ITEM_WIDGET_HEIGHT);

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
                switch (activeConnection->state())
                {
                case ActiveConnection::Activating:
                    connectionItemWidget->setLoadingStatus(true);
                    connectionItemWidget->setLabelVisible(true);
                    break;
                case ActiveConnection::Activated:
                    connectionItemWidget->activatedStatus();
                    connectionItemWidget->setEditButtonVisible(true);
                    break;
                default:
                    break;
                }
            }
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    connectionItemWidget->setProperty(PROPERTY_NETWORK_CONNECTION_INFO, var);
    connectionItemWidget->setAccessibleName(QString("WirelessConnectionItem::%1").arg(connectionInfo.id));

    this->addWidget(connectionItemWidget);
    this->setMaximumHeight((this->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * this->count());
    //this->setFixedHeight((this->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * this->count());

    connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this, &PluginConnectionList::handleEditButtonClicked);
    connect(connectionItemWidget, &ConnectionItemWidget::clicked, this, &PluginConnectionList::handleConnectionItemClicked);

    //根据signalStrength自定义排序
    this->sort();
}

void PluginConnectionList::addHiddenNetworkItemWidget()
{
    ConnectionItemWidget* connectionItemWidget = new ConnectionItemWidget();
    connectionItemWidget->setName(tr("Other WiFi networks"));
    connectionItemWidget->setEditButtonVisible(false);
    connectionItemWidget->setOtherNetworkIcon();
    connectionItemWidget->setFixedHeight(PLUGIN_ITEM_WIDGET_HEIGHT);

    NetworkConnectionInfo connectionInfo;
    connectionInfo.isWireless = true;
    // 隐藏网络信号设为－１，以方便排序在最底层
    connectionInfo.wirelessInfo.signalStrength = -1;
    QVariant var;
    var.setValue(connectionInfo);
    connectionItemWidget->setProperty(PROPERTY_NETWORK_CONNECTION_INFO, var);
    connectionItemWidget->setAccessibleName(QString("ShowOtherWireless"));

    this->addWidget(connectionItemWidget);
    this->setMaximumHeight((this->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * this->count());
    //this->setFixedHeight((this->widgetContentsSpacing() + PLUGIN_ITEM_WIDGET_HEIGHT) * this->count());

    connect(connectionItemWidget, &ConnectionItemWidget::editButtonClicked, this, &PluginConnectionList::handleEditButtonClicked);
    connect(connectionItemWidget, &ConnectionItemWidget::clicked, this, &PluginConnectionList::handleConnectionItemClicked);

    //排序
    this->sort();
}

void PluginConnectionList::handleEditButtonClicked()
{
    auto connectionItemWidget = qobject_cast<ConnectionItemWidget*>(sender());
    NetworkConnectionInfo connectionInfo = connectionItemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>();

    QString uuid = connectionInfo.uuid;
    QString activeConnectionPath = connectionInfo.activeConnectionPath;
    bool isWireless = connectionInfo.isWireless;

    KLOG_DEBUG() << "edit connection path:" << activeConnectionPath;
    if (isWireless)
    {
        if (!activeConnectionPath.isEmpty())
            emit editConnection(uuid, activeConnectionPath);
        else
            KLOG_DEBUG() << "can not edit an unconnected wireless network ";
    }
    else
        emit editConnection(uuid, activeConnectionPath);
}

void PluginConnectionList::setItemWidgetStatus(const QString& activePath, NetworkManager::ActiveConnection::State state)
{
    auto itemWidget = findItemWidgetByActivePath(activePath);
    if (itemWidget == nullptr)
    {
        KLOG_DEBUG() << "active ItemWidget was no found";
        return;
    }

    ConnectionItemWidget* connectionItemWidget = qobject_cast<ConnectionItemWidget*>(itemWidget);
    switch (state)
    {
    case ActiveConnection::State::Unknown:
        break;
    case ActiveConnection::State::Activating:
        connectionItemWidget->setLoadingStatus(true);
        connectionItemWidget->setLabelVisible(true);
        break;
    case ActiveConnection::State::Activated:
        connectionItemWidget->setLoadingStatus(false);
        connectionItemWidget->activatedStatus();
        connectionItemWidget->setEditButtonVisible(true);
        connectionItemWidget->setActiveConnectionPath(activePath);
        break;
    case ActiveConnection::State::Deactivating:
        break;
    case ActiveConnection::State::Deactivated:
        break;
    default:
        break;
    }
}

void PluginConnectionList::clearItemWidgetActiveConnectionInfo(QWidget* widget)
{
    ConnectionItemWidget* itemWidget = qobject_cast<ConnectionItemWidget*>(widget);
    itemWidget->deactivateStatus();
    bool isWireless = itemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().isWireless;
    if (isWireless)
    {
        itemWidget->setEditButtonVisible(false);
    }
    // 赋值为空，清空已激活路径
    updateItemWidgetActivePath(itemWidget, "");
}

void PluginConnectionList::handleConnectionItemClicked()
{
    auto itemWidget = qobject_cast<ConnectionItemWidget*>(sender());
    NetworkConnectionInfo connectionInfo = itemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>();
    QString activeConnectionPath = connectionInfo.activeConnectionPath;
    // 判断是否已激活
    if (activeConnectionPath.isEmpty())
    {
        bool isWireless = connectionInfo.isWireless;
        QString connectionPath = connectionInfo.connectionPath;
        if (isWireless)
        {
            if (connectionInfo.wirelessInfo.signalStrength == -1)
            {
                KLOG_DEBUG() << "connect hidden network";
                TextInputDialog ssidInputDialog;
                ssidInputDialog.setTitle(tr("Tips"));
                QString tips = QString(tr("Please input a network name"));
                ssidInputDialog.setText(tips);
                connect(&ssidInputDialog, &TextInputDialog::ssid, this, &PluginConnectionList::sendSsidToWireless);
                ssidInputDialog.exec();
            }
            else
                emit activateSelectedWirelessNetwork(connectionInfo);
        }
        else
            emit activateSelectedConnection(connectionPath);
    }
    else
        KLOG_DEBUG() << "this connection is activated";
}
