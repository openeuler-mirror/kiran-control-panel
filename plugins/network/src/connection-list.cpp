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
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */
#include "connection-list.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <QCollator>
#include "general.h"

using namespace NetworkManager;

ConnectionList::ConnectionList(QWidget *parent) : QWidget(parent)
{
    qRegisterMetaType<NetworkConnectionInfo>("NetworkConnectionInfo");
    initUI();
    initConnect();
}

ConnectionList::~ConnectionList()
{
}

void ConnectionList::initUI()
{
    m_verticalLayout = new QVBoxLayout(this);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_scrollAreaWidgetContents = new QWidget();
    m_widgetContentsLayout = new QVBoxLayout(m_scrollAreaWidgetContents);
    m_widgetContentsLayout->setContentsMargins(0, 0, 0, 0);
    m_widgetContentsLayout->setSpacing(0);

    // QScrollArea::setWidget会更改传入的widget的parentWidget
    m_scrollArea->setWidget(m_scrollAreaWidgetContents);

    m_verticalLayout->addWidget(m_scrollArea);
    m_verticalLayout->setContentsMargins(0, 0, 0, 0);
    m_verticalLayout->setSpacing(0);
}

void ConnectionList::initConnect()
{
}

void ConnectionList::setDevicePath(const QString &devicePath)
{
    m_devicePath = devicePath;
}

void ConnectionList::addWidget(QWidget *widget)
{
    m_widgetContentsLayout->addWidget(widget);
    m_itemWidgetList << widget;
}

QWidget *ConnectionList::itemWidget(int row)
{
    return m_itemWidgetList.value(row);
}

QList<QWidget *> ConnectionList::itemWidgetList()
{
    return m_itemWidgetList;
}

void ConnectionList::showConnectionList(NetworkManager::ConnectionSettings::ConnectionType type)
{
    Connection::List connectionList = NetworkManager::listConnections();
    if (type == ConnectionSettings::Wired)
    {
        Device::Ptr device = findNetworkInterface(m_devicePath);
        QString devicePath = device->uni();
        for (Connection::Ptr conn : connectionList)
        {
            KLOG_DEBUG() << "connection->name():" << conn->name();
            KLOG_DEBUG() << "connection->settings()->connectionType():" << conn->settings()->connectionType();
            if (conn->settings()->connectionType() == ConnectionSettings::Wired)
            {
                KLOG_DEBUG() << "deviceName:" << device->interfaceName();
                addConnection(conn, devicePath);
            }
        }
    }
    else if (type == ConnectionSettings::Vpn)
    {
        // VPN的设备不明,VPN暂不指定设备
        Connection::List connectionList = NetworkManager::listConnections();
        for (Connection::Ptr conn : connectionList)
        {
            if (conn->settings()->connectionType() == ConnectionSettings::Vpn)
                addConnection(conn, "");
        }
    }
}

void ConnectionList::showWirelessNetworkList()
{
    Device::Ptr device = findNetworkInterface(m_devicePath);
    if (device->type() == Device::Wifi)
    {
        KLOG_DEBUG() << "dev->interfaceName():" << device->interfaceName();
        QSharedPointer<WirelessDevice> wirelessDevice = qobject_cast<WirelessDevice *>(device);
        WirelessNetwork::List wirelessNetworkList = wirelessDevice->networks();
        QString devicePath = wirelessDevice->uni();
        for (WirelessNetwork::Ptr network : wirelessNetworkList)
        {
            addWirelessNetwork(network, devicePath);
        }
        addHiddenNetworkItemWidget();
    }
}

void ConnectionList::addHiddenNetworkItemWidget()
{
}

void ConnectionList::addConnection(NetworkManager::Connection::Ptr ptr, const QString &devicePath)
{
}

void ConnectionList::addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network,
                                        const QString &devicePath)
{
}

void ConnectionList::removeConnectionFromList(const QString &path)
{
    for (int i = 0; i < m_itemWidgetList.count(); ++i)
    {
        QWidget *widget = m_itemWidgetList.value(i);
        QString itemPath = widget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().connectionPath;
        if (path == itemPath)
        {
            m_itemWidgetList.removeAt(i);
            widget->deleteLater();
            break;
        }
    }
}

void ConnectionList::removeWirelessNetworkFromList(const QString &ssid)
{
    for (int i = 0; i < m_itemWidgetList.count(); ++i)
    {
        QWidget *widget = m_itemWidgetList.value(i);
        QString itemSsid = widget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            m_itemWidgetList.removeAt(i);
            widget->deleteLater();
            break;
        }
    }
}

void ConnectionList::clearConnectionList()
{
    int count = m_itemWidgetList.count();
    for (int i = 0; i < count; ++i)
    {
        QWidget *itemWidget = m_itemWidgetList.value(i);
        m_itemWidgetList.removeAt(i);
        itemWidget->deleteLater();
    }
}

void ConnectionList::updateItemWidgetActivePath(QWidget *itemWidget,
                                                QString activePath)
{
    NetworkConnectionInfo connectionInfo;
    connectionInfo = itemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>();
    connectionInfo.activeConnectionPath = activePath;
    QVariant var;
    var.setValue(connectionInfo);
    itemWidget->setProperty(PROPERTY_NETWORK_CONNECTION_INFO, var);
}

void ConnectionList::handleConnectionUpdated()
{
    auto ptr = qobject_cast<Connection *>(sender());
    emit connectionUpdated(ptr->path());
}

void ConnectionList::handleConnectionItemClicked()
{
}

QWidget *ConnectionList::findItemWidgetByUuid(const QString &uuid)
{
    for (int i = 0; i < m_itemWidgetList.count(); ++i)
    {
        QWidget *itemWidget = m_itemWidgetList.value(i);
        QString itemUuid = itemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().uuid;
        if (uuid == itemUuid)
        {
            return itemWidget;
        }
    }
    return nullptr;
}

QWidget *ConnectionList::findItemWidgetBySsid(const QString &ssid)
{
    for (int i = 0; i < m_itemWidgetList.count(); ++i)
    {
        QWidget *itemWidget = m_itemWidgetList.value(i);
        QString itemSsid = itemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            return itemWidget;
        }
    }
    return nullptr;
}

QWidget *ConnectionList::findItemWidgetByActivePath(const QString &activePath)
{
    for (int i = 0; i < m_itemWidgetList.count(); ++i)
    {
        QWidget *itemWidget = m_itemWidgetList.value(i);
        QString activeConnectionPathFromItem = itemWidget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath;
        if (activePath == activeConnectionPathFromItem)
        {
            return itemWidget;
        }
    }
    return nullptr;
}

void ConnectionList::handleActiveStateDeactivated(const QString &activeConnectionPath)
{
    auto *activeItemWidget = findItemWidgetByActivePath(activeConnectionPath);
    // 没有找到item则直接返回
    if (activeItemWidget == nullptr)
    {
        KLOG_DEBUG() << "Activated item was no found";
        return;
    }
    clearItemWidgetActiveConnectionInfo(activeItemWidget);
}

void ConnectionList::clearItemWidgetActiveConnectionInfo(QWidget *itemWidget)
{
}

int ConnectionList::count()
{
    return m_itemWidgetList.count();
}

void ConnectionList::setWidgetContentsSpacing(int spacing)
{
    m_widgetContentsLayout->setSpacing(spacing);
}

int ConnectionList::widgetContentsSpacing()
{
    return m_widgetContentsLayout->spacing();
}

void ConnectionList::setWidgetContentsMargins(int left, int top, int right, int bottom)
{
    m_widgetContentsLayout->setContentsMargins(left, top, right, bottom);
}

QMargins ConnectionList::contentsMargins() const
{
    return m_widgetContentsLayout->contentsMargins();
}

//无线网络根据signalStrength排序，有线网络根据名称排序
//已连接的网络将排在最上面
void ConnectionList::sort()
{
    using std::sort;

    if (m_itemWidgetList.count() == 0)
    {
        KLOG_DEBUG() << "Sorting failed, connection list cannot be empty.";
        return;
    }

    auto widget = m_itemWidgetList.value(0);
    bool isWireless = widget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().isWireless;
    if (isWireless)
    {
        QVector<QPair<QWidget *, int> > sorting;
        for (auto widget : m_itemWidgetList)
        {
            int signalStrength = widget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().wirelessInfo.signalStrength;
            QPair<QWidget *, int> pair;
            pair.first = widget;
            pair.second = signalStrength;
            sorting << pair;
        }
        std::sort(sorting.begin(), sorting.end(), greaterThan);
        clearContentsWidget();
        for (auto pair : sorting)
        {
            pair.first;
            addWidget(pair.first);
        }
    }
    else
    {
        QVector<QPair<QWidget *, QString> > sortingString;
        for (auto widget : m_itemWidgetList)
        {
            QString id = widget->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().id;
            QPair<QWidget *, QString> pair;
            pair.first = widget;
            pair.second = id;
            sortingString << pair;
        }
        std::sort(sortingString.begin(), sortingString.end(), greaterThanString);
        clearContentsWidget();
        for (auto pair : sortingString)
        {
            pair.first;
            addWidget(pair.first);
        }
    }
}

bool ConnectionList::lessThan(const QPair<QWidget *, int> &left, const QPair<QWidget *, int> &right)
{
    return left.second < right.second;
}

bool ConnectionList::greaterThan(const QPair<QWidget *, int> &left, const QPair<QWidget *, int> &right)
{
    if (!left.first->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
        return 1;

    if (!right.first->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
        return 0;

    return left.second > right.second;
}

bool ConnectionList::lessThanString(const QPair<QWidget *, QString> &left, const QPair<QWidget *, QString> &right)
{
    QCollator collator;
    collator.setNumericMode(true);
    int bInt = collator.compare(left.second, right.second);
    if (bInt != 1)
        bInt = 0;

    return bInt;
}

bool ConnectionList::greaterThanString(const QPair<QWidget *, QString> &left, const QPair<QWidget *, QString> &right)
{
    if (!left.first->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
        return 1;

    if (!right.first->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath.isEmpty())
        return 0;

    right.first->property(PROPERTY_NETWORK_CONNECTION_INFO).value<NetworkConnectionInfo>().activeConnectionPath;

    QCollator collator;
    collator.setNumericMode(true);
    int bInt = collator.compare(left.second, right.second);
    bInt = (bInt == 1) ? 0 : 1;

    return bInt;
}

void ConnectionList::clearContentsWidget()
{
    for (auto widget : m_itemWidgetList)
    {
        m_widgetContentsLayout->removeWidget(widget);
    }
    m_itemWidgetList.clear();
}
