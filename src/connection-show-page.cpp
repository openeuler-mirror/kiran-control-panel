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

#include "connection-show-page.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <QHBoxLayout>
#include <QPainter>
#include <QSvgRenderer>
#include "animation-loading-label.h"
#include "ui_connection-show-page.h"

ConnectionShowPage::ConnectionShowPage(QWidget* parent) : QWidget(parent), ui(new Ui::ConnectionShowPage)
{
    ui->setupUi(this);
    initUI();
    initConnect();
}

ConnectionShowPage::~ConnectionShowPage()
{
    KLOG_DEBUG() << "ConnectionShowPage::~ConnectionShowPage()";
    delete ui;
}

void ConnectionShowPage::setTitle(QString title)
{
    ui->title->setText(title);
}

void ConnectionShowPage::initUI()
{
    m_switchButton = new KiranSwitchButton(this);
    ui->titleLayout->addWidget(m_switchButton);
}

void ConnectionShowPage::initConnect()
{
    connect(ui->createConnectionButton, &QPushButton::clicked, [=]() { emit requestCreatConnection(); });
    connect(ui->connectionLists, &QListWidget::itemClicked, this, &ConnectionShowPage::handleConnectionItemClicked);
}

void ConnectionShowPage::handleConnectionItemClicked(QListWidgetItem* item)
{
    //判断是否已激活
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

void ConnectionShowPage::setSwitchButtonVisible(bool visible)
{
    m_switchButton->setVisible(visible);
}

void ConnectionShowPage::showConnectionLists(ConnectionSettings::ConnectionType type)
{
    const Device::List deviceList = networkInterfaces();
    Connection::List connectionList = listConnections();
    KLOG_DEBUG() << "deviceList:" << deviceList;

    if (type == ConnectionSettings::Wired)
    {
        for (Device::Ptr dev : deviceList)
        {
            if (dev->type() == Device::Ethernet)
            {
                QSharedPointer<WiredDevice> wiredDevice = qobject_cast<WiredDevice*>(dev);
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
        }
    }
    else if (type == ConnectionSettings::Vpn)
    {
        for (Connection::Ptr conn : connectionList)
        {
            if (conn->settings()->connectionType() == ConnectionSettings::Vpn)
                addConnectionToLists(conn, "");
        }
    }
    //有线网络是否需要排序
    //    ui->connectionLists->sortItems();
}

//VPN的设备不明,VPN暂不指定设备
void ConnectionShowPage::addConnectionToLists(Connection::Ptr ptr, const QString& devicePath)
{
    ItemWidget* itemWidget = new ItemWidget();
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
        if (activeConnection->uuid() == ptr->uuid())
        {
            connectionInfo.activeConnectionPath = activeConnection->path();
            connect(activeConnection.data(), &ActiveConnection::stateChanged, [=](ActiveConnection::State state) {
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
                }
            });

            itemWidget->activatedLabel();
            m_previousActivatedItem = item;
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    //item中保存connection的相关信息
    item->setData(Qt::UserRole, var);
    ui->connectionLists->addItem(item);
    ui->connectionLists->setItemWidget(item, itemWidget);
    ui->connectionLists->setMaximumHeight(ui->connectionLists->sizeHintForRow(0) * ui->connectionLists->count() + (2 * ui->connectionLists->frameWidth()));

    connect(itemWidget, &ItemWidget::editConnectionClicked, [=]() {
        QString uuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        KLOG_DEBUG() << "uuid:" << uuid;
        KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
        emit requestEditConnection(uuid, activeConnectionPath);
    });
}

void ConnectionShowPage::showWirelessNetworkLists()
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
    ui->connectionLists->sortItems();
}

void ConnectionShowPage::addWirelessNetworkToLists(WirelessNetwork::Ptr network, const QString& devicePath)
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

    ItemWidget* itemWidget = new ItemWidget();
    itemWidget->setName(network->ssid());
    itemWidget->setWirelessLabel(connectionInfo.isWireless, connectionInfo.wirelessInfo.signalStrength);
    CustomSortListItem* item = new CustomSortListItem();
    item->setSizeHint(QSize(200, 50));

    //已连接的情况
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
                connect(activeConnection.data(), &ActiveConnection::stateChanged, [=](ActiveConnection::State state) {
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
                    }
                });
                itemWidget->activatedLabel();
                m_previousActivatedItem = item;
            }
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
    ui->connectionLists->addItem(item);
    ui->connectionLists->setItemWidget(item, itemWidget);
    ui->connectionLists->setMaximumHeight(ui->connectionLists->sizeHintForRow(0) * ui->connectionLists->count() + (2 * ui->connectionLists->frameWidth()));

    connect(itemWidget, &ItemWidget::editConnectionClicked, [=]() {
        QString uuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
        if(!activeConnectionPath.isEmpty())
            emit requestEditConnection(uuid, activeConnectionPath);
        else
            KLOG_DEBUG() << "can not edit an unconnected wireless network ";
    });

}

void ConnectionShowPage::removeConnectionFromLists(const QString& path)
{
    for (int i = 0; i < ui->connectionLists->count(); ++i)
    {
        QListWidgetItem* item = ui->connectionLists->item(i);
        QString itemPath = item->data(Qt::UserRole).value<ConnectionInfo>().connectionPath;
        if (path == itemPath)
        {
            ui->connectionLists->takeItem(i);
            //使用takeItem:Items removed from a list widget will not be managed by Qt, and will need to be deleted manually.
            QWidget* itemWidget = ui->connectionLists->itemWidget(item);
            itemWidget->deleteLater();
            delete item;
            break;
        }
    }
}

/**
  通过connectionLists->clear()释放所有的Item项
  但插入Item中的Widget并不会一起释放，需要另外释放
*/
void ConnectionShowPage::clearConnectionLists()
{
    KLOG_DEBUG() << "clearConnectionLists()";
    int itemCount = ui->connectionLists->count();
    for (int i = 0; i < itemCount; ++i)
    {
        QListWidgetItem* item = ui->connectionLists->item(i);
        QWidget* itemWidget = ui->connectionLists->itemWidget(item);
        itemWidget->deleteLater();
    }
    ui->connectionLists->clear();
    m_previousActivatedItem = nullptr;
}

void ConnectionShowPage::findItemByUuid(const QString& uuid)
{
    for (int i = 0; i < ui->connectionLists->count(); ++i)
    {
        QListWidgetItem* item = ui->connectionLists->item(i);
        QString itemUuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        if (uuid == itemUuid)
        {
            m_currentActiveItem = item;
            break;
        }
    }
}

void ConnectionShowPage::findItemBySsid(const QString& ssid)
{
    for (int i = 0; i < ui->connectionLists->count(); ++i)
    {
        QListWidgetItem* item = ui->connectionLists->item(i);
        QString itemSsid = item->data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.ssid;
        if (ssid == itemSsid)
        {
            m_currentActiveItem = item;
            break;
        }
    }
}

//Note:不应该通过点击item来加载动画，而应该通过信号来激活加载动画
void ConnectionShowPage::connectionItemLoadingAnimation()
{
    QWidget* widget = ui->connectionLists->itemWidget(m_currentActiveItem);
    ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
    itemWidget->setLoadingStatus(true);
    itemWidget->setLabelVisible(true);
}

void ConnectionShowPage::updateActivatedConnectionInfo(QString activatedPath)
{
    m_previousActivatedItem = m_currentActiveItem;
    QWidget* widget = ui->connectionLists->itemWidget(m_currentActiveItem);
    ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
    itemWidget->setLoadingStatus(false);
    itemWidget->activatedLabel();

    updateItemActivatedPath(m_currentActiveItem, activatedPath);
}

void ConnectionShowPage::updateItemActivatedPath(QListWidgetItem* item, QString activatedPath)
{
    ConnectionInfo connectionInfo;
    connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
    connectionInfo.activeConnectionPath = activatedPath;
    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
}

void ConnectionShowPage::clearDeactivatedConnectionInfo(const QString& deactivatedPath)
{
    if (m_previousActivatedItem != nullptr)
    {
        QWidget* widget = ui->connectionLists->itemWidget(m_previousActivatedItem);
        ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
        itemWidget->deactivateLabel();
        //清空已激活路径,更新item所带信息
        updateItemActivatedPath(m_previousActivatedItem, "");
        m_previousActivatedItem = nullptr;
    }
    else
    {
        //对应网络激活过程中激活失败，直接断开的情况
        QWidget* widget = ui->connectionLists->itemWidget(m_currentActiveItem);
        ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
        itemWidget->deactivateLabel();
    }
}

//使用默认析够函数，父对象被释放时，会释放子对象
ItemWidget::ItemWidget(QWidget* parent) : QWidget(parent)
{
    m_wirelessLabel = new QLabel(this);
    m_connectionName = new QLabel(this);
    m_horizonLayout = new QHBoxLayout(this);
    m_editConnection = new QPushButton(this);
    m_activatedLabel = new AnimationLoadingLabel(this);

    m_wirelessLabel->setVisible(false);
    m_activatedLabel->setVisible(false);

    m_horizonLayout->addWidget(m_wirelessLabel);
    m_horizonLayout->addWidget(m_connectionName);
    m_horizonLayout->addStretch();
    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_editConnection);
    m_horizonLayout->setMargin(0);

    this->setLayout(m_horizonLayout);
    connect(m_editConnection, &QPushButton::clicked, this, &ItemWidget::editConnectionClicked);
}

void ItemWidget::setName(const QString& name)
{
    m_connectionName->setText(name);
}

void ItemWidget::activatedLabel()
{
    QPixmap pixmap = getPixmapFromSvg("/home/luoqing/git/icon_correct.svg");
    m_activatedLabel->setPixmap(pixmap);
    m_activatedLabel->setAlignment(Qt::AlignCenter);
    m_activatedLabel->setVisible(true);
}

void ItemWidget::deactivateLabel()
{
    m_activatedLabel->setVisible(false);
}

void ItemWidget::setLoadingStatus(bool isLoading)
{
    m_activatedLabel->setLoadingStatus(isLoading);
}

void ItemWidget::setLabelVisible(bool isVisible)
{
    m_activatedLabel->setVisible(isVisible);
}

void ItemWidget::setWirelessLabel(bool security, int signal)
{
    QString svgPath;
    if (security)
    {
        if (0 <= signal && signal < 25)
            svgPath = ":/kcp-network-images/wireless-1-security.svg";
        else if (25 <= signal && signal < 50)
            svgPath = ":/kcp-network-images/wireless-2-security.svg";
        else if (50 <= signal && signal < 75)
            svgPath = ":/kcp-network-images/wireless-3-security.svg";
        else if (75 <= signal && signal <= 100)
            svgPath = ":/kcp-network-images/wireless-4-security.svg";
    }
    else
    {
        if (0 <= signal && signal < 25)
            svgPath = ":/kcp-network-images/wireless-1.svg";
        else if (25 <= signal && signal < 50)
            svgPath = ":/kcp-network-images/wireless-2.svg";
        else if (50 <= signal && signal < 75)
            svgPath = ":/kcp-network-images/wireless-3.svg";
        else if (75 <= signal && signal <= 100)
            svgPath = ":/kcp-network-images/wireless-4.svg";
    }
    KLOG_DEBUG() << "svgPath" << svgPath;
    QPixmap pixmap = getPixmapFromSvg(svgPath);
    m_wirelessLabel->setPixmap(pixmap);
    m_wirelessLabel->setAlignment(Qt::AlignCenter);
    m_wirelessLabel->setVisible(true);
}

QPixmap ItemWidget::getPixmapFromSvg(const QString& svgPath)
{
    QSvgRenderer svgRenderer(QString(svgPath), this);
    QPixmap pixmap(QSize(16, 16));
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    svgRenderer.render(&painter);
    return pixmap;
}

CustomSortListItem::CustomSortListItem(QWidget* parent)
{
}

bool CustomSortListItem::operator<(const QListWidgetItem& other) const
{
    int left = this->data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.signalStrength;
    int right = other.data(Qt::UserRole).value<ConnectionInfo>().wirelessInfo.signalStrength;
    return left > right;
    //    return QListWidgetItem::operator<(other);
}
