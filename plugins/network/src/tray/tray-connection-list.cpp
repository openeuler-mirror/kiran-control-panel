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

#include "tray-connection-list.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/WirelessSetting>
#include "general.h"
#include "status-notification.h"
#include "tray-itemwidget.h"

#define LIST_MAX_HEIGHT 358
#define TRAY_ITEM_NORAML_SIZE QSize(240, 50)
#define TRAY_ITEM_EXTENDED_SIZE QSize(240, 100)

using namespace NetworkManager;

TrayConnectionList::TrayConnectionList(QWidget* parent) : ConnectionList(parent)
{
    initUI();
    initConnect();
}

TrayConnectionList::~TrayConnectionList()
{
}

void TrayConnectionList::initUI()
{
}

void TrayConnectionList::initConnect()
{
}

void TrayConnectionList::addConnection(NetworkManager::Connection::Ptr ptr, const QString& devicePath)
{
    if (ptr == nullptr)
    {
        KLOG_ERROR() << "ptr == null";
        return;
    }

    NetworkConnectionInfo connectionInfo;
    connectionInfo.id = ptr->name();
    connectionInfo.uuid = ptr->uuid();
    connectionInfo.connectionPath = ptr->path();
    connectionInfo.devicePath = devicePath;

    TrayItemWidget* trayItemWidget = new TrayItemWidget();
    trayItemWidget->setName(ptr->name());
    trayItemWidget->setWiredStatusIcon();
    trayItemWidget->setFixedSize(TRAY_ITEM_NORAML_SIZE);
    trayItemWidget->setWiredStatusIcon();

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
                trayItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
                trayItemWidget->activatingStatus();
                break;
            case ActiveConnection::Activated:
                trayItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
                trayItemWidget->activatedStatus();
                break;
            default:
                break;
            }
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    // item中保存connection的相关信息
    trayItemWidget->setProperty(PROPERTY_NETWORK_CONNECTION_INFO, var);
    trayItemWidget->setAccessibleName(QString("WiredConnectionItem::%1").arg(connectionInfo.id));

    this->addWidget(trayItemWidget);
    this->sort();
    adjustTraySize();

    connect(ptr.data(), &Connection::updated, this, &TrayConnectionList::handleConnectionUpdated, Qt::UniqueConnection);

    connect(trayItemWidget, &TrayItemWidget::disconnectButttonClicked, this, &TrayConnectionList::handleDisconnectButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this, &TrayConnectionList::handleConnectButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this, &TrayConnectionList::handleCancelButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::clicked, this, &TrayConnectionList::handleConnectionItemClicked);
}

void TrayConnectionList::addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network,
                                            const QString& devicePath)

{
    // KLOG_DEBUG() << "network ssid:" << network->ssid();
    AccessPoint::Ptr accessPoint = network->referenceAccessPoint();
    NetworkConnectionInfo connectionInfo;
    connectionInfo.isWireless = true;
    connectionInfo.wirelessInfo.ssid = network->ssid();
    connectionInfo.wirelessInfo.accessPointPath = accessPoint->uni();
    connectionInfo.wirelessInfo.signalStrength = accessPoint->signalStrength();
    // KLOG_DEBUG() << "accessPoint signalStrength:" << connectionInfo.wirelessInfo.signalStrength;
    connectionInfo.devicePath = devicePath;
    if (accessPoint->capabilities() == AccessPoint::Capability::None)
        connectionInfo.wirelessInfo.securitySetting = false;
    else
        connectionInfo.wirelessInfo.securitySetting = true;

    TrayItemWidget* trayItemWidget = new TrayItemWidget();
    trayItemWidget->setName(network->ssid());
    trayItemWidget->setWirelessStatusIcon(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
    trayItemWidget->setFixedSize(TRAY_ITEM_NORAML_SIZE);

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
                    trayItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
                    trayItemWidget->activatingStatus();
                    break;
                case ActiveConnection::Activated:
                    trayItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
                    trayItemWidget->activatedStatus();
                    trayItemWidget->setIgnoreButtonVisible(true);
                    break;
                default:
                    break;
                }
            }
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    trayItemWidget->setProperty(PROPERTY_NETWORK_CONNECTION_INFO, var);
    trayItemWidget->setAccessibleName(QString("WirelessConnectionItem::%1").arg(connectionInfo.id));

    this->addWidget(trayItemWidget);

    adjustTraySize();

    connect(trayItemWidget, &TrayItemWidget::disconnectButttonClicked, this, &TrayConnectionList::handleDisconnectButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this, &TrayConnectionList::handleConnectButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this, &TrayConnectionList::handleCancelButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::ignoreButtonClicked, this, &TrayConnectionList::handleIgnoreButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::clicked, this, &TrayConnectionList::handleConnectionItemClicked);

    // 根据signalStrength自定义排序
    this->sort();
}

void TrayConnectionList::addHiddenNetworkItemWidget()
{
    TrayItemWidget* trayItemWidget = nullptr;

    trayItemWidget = new TrayItemWidget();
    trayItemWidget->setName(tr("Other WiFi networks"));
    trayItemWidget->setOtherNetworkIcon();
    trayItemWidget->setFixedSize(TRAY_ITEM_NORAML_SIZE);

    NetworkConnectionInfo connectionInfo;
    connectionInfo.isWireless = true;
    // 隐藏网络信号设为－１，以方便排序在最底层
    connectionInfo.wirelessInfo.signalStrength = -1;
    QVariant var;
    var.setValue(connectionInfo);
    trayItemWidget->setProperty(PROPERTY_NETWORK_CONNECTION_INFO, var);
    trayItemWidget->setAccessibleName(QString("ShowOtherWireless"));

    this->addWidget(trayItemWidget);
    adjustTraySize();

    connect(trayItemWidget, &TrayItemWidget::connectButtonClicked, this, &TrayConnectionList::handleConnectButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::cancelButtonClicked, this, &TrayConnectionList::handleCancelButtonClicked);
    connect(trayItemWidget, &TrayItemWidget::sendSsid, this, &TrayConnectionList::sendSsidToWireless);
    connect(trayItemWidget, &TrayItemWidget::clicked, this, &TrayConnectionList::handleConnectionItemClicked);

    this->sort();
}

void TrayConnectionList::setItemWidgetStatus(const QString& activePath, NetworkManager::ActiveConnection::State state)
{
    KLOG_DEBUG() << "activePath:" << activePath;
    auto itemWidget = findItemWidgetByActivePath(activePath);
    if (itemWidget == nullptr)
    {
        KLOG_DEBUG() << "active ItemWidget was no found";
        return;
    }

    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(itemWidget);
    auto connectionInfo = trayItemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>();
    switch (state)
    {
    case ActiveConnection::State::Unknown:
        break;
    case ActiveConnection::State::Activating:
        trayItemWidget->activatingStatus();
        trayItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
        break;
    case ActiveConnection::State::Activated:
        trayItemWidget->setLoadingStatus(false);
        trayItemWidget->activatedStatus();
        trayItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
        if (connectionInfo.isWireless)
            trayItemWidget->setIgnoreButtonVisible(true);
        break;
    case ActiveConnection::State::Deactivating:
        break;
    case ActiveConnection::State::Deactivated:
        break;
    default:
        break;
    }
    adjustTraySize();
}

void TrayConnectionList::clearItemWidgetActiveConnectionInfo(QWidget* itemWidget)
{
    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(itemWidget);
    trayItemWidget->deactivateStatus();
    trayItemWidget->setEnableConnectButton(true);
    trayItemWidget->setFixedSize(TRAY_ITEM_NORAML_SIZE);
    adjustTraySize();

    // 赋值为空，清空已激活路径
    updateItemWidgetActivePath(trayItemWidget, "");
}

void TrayConnectionList::adjustTraySize()
{
    // 由于托盘每一项都可以伸缩扩展，遍历所有widget以计算高度
    int totalheight = 0;
    QList<QWidget*> widgetList = itemWidgetList();
    for (auto widget : widgetList)
    {
        totalheight += widget->height();
    }

    if (totalheight > LIST_MAX_HEIGHT)
        totalheight = LIST_MAX_HEIGHT;
    setFixedHeight(totalheight);

    // KLOG_DEBUG() << "totalheight:" << totalheight;
    emit sizeChanged(QSize(this->sizeHint().width(), totalheight));
}

void TrayConnectionList::handleConnectionItemClicked()
{
    TrayItemWidget* clickedItemWidget = qobject_cast<TrayItemWidget*>(sender());
    NetworkConnectionInfo connectionInfo = clickedItemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>();
    QString activeConnectionPath = connectionInfo.activeConnectionPath;

    if (activeConnectionPath.isEmpty())
    {
        bool isWireless = connectionInfo.isWireless;
        QString connectionPath = connectionInfo.connectionPath;

        // 若itemWidget的size为EXTENDED_SIZE,说明itemWidget已经展开，并进入了激活操作流程
        if (clickedItemWidget->size() != TRAY_ITEM_EXTENDED_SIZE)
        {
            //调整itemWidget的height，将其展开
            clickedItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
            if (isWireless)
            {
                // Note:signalStrength == -1 对应连接隐藏网络
                if (connectionInfo.wirelessInfo.signalStrength == -1)
                {
                    clickedItemWidget->showInputSsidWidget();
                }
                else
                    clickedItemWidget->prepareConnectStatus();
            }
            else
                clickedItemWidget->prepareConnectStatus();

            //将其它展开但是未激活的itemWidget收缩回去
            QList<QWidget*> widgetList = itemWidgetList();
            for (auto widget : widgetList)
            {
                if (widget != clickedItemWidget)
                {
                    // activeConnectionPath 为空代表未激活
                    if (widget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
                    {
                        if (widget->size() != TRAY_ITEM_NORAML_SIZE)
                        {
                            setItemWidgetSimpleStatus(widget);
                        }
                    }
                }
            }
            adjustTraySize();
        }
    }
    else
        KLOG_DEBUG() << "this connection is activated";
}

void TrayConnectionList::setItemWidgetSimpleStatus(QWidget* itemWidget)
{
    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(itemWidget);
    trayItemWidget->simpleStatus();
    trayItemWidget->setFixedSize(TRAY_ITEM_NORAML_SIZE);
}

void TrayConnectionList::handleConnectButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    auto connectionInfo = trayItemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>();
    if (connectionInfo.isWireless)
        emit activateSelectedWirelessNetwork(connectionInfo);
    else
        emit activateSelectedConnection(connectionInfo.connectionPath);
}

void TrayConnectionList::handleDisconnectButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    QString activeConnectionPath = trayItemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath;
    emit disconnect(activeConnectionPath);
}

void TrayConnectionList::handleCancelButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    trayItemWidget->simpleStatus();
    trayItemWidget->setFixedSize(TRAY_ITEM_NORAML_SIZE);
    adjustTraySize();

    QString activeConnectionPathFromItem = trayItemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath;
    if (!activeConnectionPathFromItem.isEmpty())
    {
        auto activeConnectionsPathsLists = activeConnectionsPaths();
        for (QString path : activeConnectionsPathsLists)
        {
            if (activeConnectionPathFromItem == path)
            {
                ActiveConnection::Ptr activeConnection = findActiveConnection(path);
                if(activeConnection.isNull())
                {
                    break;
                }
                if (activeConnection->state() == ActiveConnection::Activating)
                {
                    emit cancelConnection(path);
                    KLOG_DEBUG() << "emit cancel Connection";
                }
            }
        }
    }
}

void TrayConnectionList::handleIgnoreButtonClicked()
{
    auto trayItemWidget = qobject_cast<TrayItemWidget*>(sender());
    trayItemWidget->simpleStatus();
    trayItemWidget->setFixedSize(TRAY_ITEM_NORAML_SIZE);
    adjustTraySize();

    QString activeConnectionPath = trayItemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath;
    emit ignoreConnection(activeConnectionPath);
}

void TrayConnectionList::showPasswordEdit(QWidget* itemWidget)
{
    TrayItemWidget* trayItemWidget = qobject_cast<TrayItemWidget*>(itemWidget);
    trayItemWidget->showInputPasswordWidget();
    trayItemWidget->setFixedSize(TRAY_ITEM_EXTENDED_SIZE);
    adjustTraySize();
    connect(trayItemWidget, &TrayItemWidget::sendPassword, this, &TrayConnectionList::handleSendPassword, Qt::UniqueConnection);
}

void TrayConnectionList::handleSendPassword(const QString& password)
{
    auto widget = qobject_cast<QWidget*>(sender());
    int hiddenNetworkRow = this->count() - 1;
    QWidget* hiddenNetworkWidget = this->itemWidget(hiddenNetworkRow);
    if (widget == hiddenNetworkWidget)
    {
        this->setItemWidgetSimpleStatus(widget);
    }
    emit sendPasswordToWirelessSetting(password);
}

void TrayConnectionList::hideEvent(QHideEvent* event)
{
    QList<QWidget*> widgetList = itemWidgetList();
    for (auto widget : widgetList)
    {
        // activeConnectionPath 为空代表未激活
        if (widget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
        {
            if (widget->size() != TRAY_ITEM_NORAML_SIZE)
            {
                setItemWidgetSimpleStatus(widget);
            }
        }
    }
    adjustTraySize();

    QWidget::hideEvent(event);
}
