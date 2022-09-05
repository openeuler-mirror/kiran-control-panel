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

#include "wired-tray-widget.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include "status-notification.h"
#include "utils.h"
using namespace NetworkManager;

WiredTrayWidget::WiredTrayWidget(const QString &devicePath, QWidget *parent) : TrayWidget(parent),
                                                                               m_connectionLists(nullptr),
                                                                               m_unavailableWidget(nullptr)
{
    m_devicePath = devicePath;
    init();
}

WiredTrayWidget::~WiredTrayWidget()
{
}

void WiredTrayWidget::init()
{
    m_devicePtr = findNetworkInterface(m_devicePath);
    m_wiredDevice = qobject_cast<WiredDevice *>(m_devicePtr);

    initUI();
    initConnection();
    ActiveConnection::Ptr activatedConnection = m_devicePtr->activeConnection();
    if (!activatedConnection.isNull())
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredTrayWidget::handleActiveConnectionStateChanged, Qt::UniqueConnection);
}

void WiredTrayWidget::initConnection()
{
    connect(m_connectionLists, &ConnectionLists::connectionUpdated, this, [this](const QString &path)
            {
        m_connectionLists->removeConnectionFromLists(path);
        Connection::Ptr updateConnection = findConnection(path);
        m_connectionLists->addConnectionToLists(updateConnection,""); });

    connect(m_connectionLists, &ConnectionLists::trayRequestConnect, this, &WiredTrayWidget::handleRequestActivateConnection);
    connect(m_connectionLists, &ConnectionLists::trayRequestDisconnect, this, &WiredTrayWidget::handleRequestDisconnect);
    connect(m_connectionLists, &ConnectionLists::trayRequestCancel, this, &WiredTrayWidget::handleRequestCancel);
    connect(m_connectionLists, &ConnectionLists::adjustedTraySize, this, &WiredTrayWidget::adjustedTraySize);

    connect(m_wiredDevice.data(), &WiredDevice::carrierChanged, this, &WiredTrayWidget::handleCarrierChanged, Qt::UniqueConnection);
    connect(m_wiredDevice.data(), &Device::stateChanged, this, &WiredTrayWidget::handleStateChanged, Qt::UniqueConnection);

    //在插拔网线时，deviceAdded和deviceRemoved信号失效
    connect(notifier(), &Notifier::deviceAdded, this, [=](const QString &uni)
            { KLOG_DEBUG() << "-----------WiredTrayWidget::deviceAdded:" << uni; });
    connect(notifier(), &Notifier::deviceRemoved, this, [=](const QString &uni)
            { KLOG_DEBUG() << "-----------WiredTrayWidget::deviceRemoved:" << uni; });
}

void WiredTrayWidget::initUI()
{
    setFixedWidth(240);
    setContentsMargins(0, 0, 0, 0);
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setSpacing(0);
    m_verticalLayout->setContentsMargins(0, 0, 0, 0);

    // m_wiredDevice->state(); 设备状态在最开始初始化托盘页面时已经判断过了
    KLOG_DEBUG() << "m_wiredDevice->carrier():" << m_wiredDevice->carrier();

    m_connectionLists = new ConnectionLists(this);
    m_verticalLayout->addWidget(m_connectionLists);

    initUnavailableWidget();
    m_verticalLayout->addWidget(m_unavailableWidget);

    if (m_wiredDevice->carrier())
    {
        m_unavailableWidget->setVisible(false);
        m_connectionLists->setVisible(true);
        showWiredConnectionLists();
    }
    else
    {
        m_unavailableWidget->setVisible(true);
        m_connectionLists->setVisible(false);
    }
}

void WiredTrayWidget::showWiredConnectionLists()
{
    m_connectionLists->setDevicePath(m_devicePath);
    m_connectionLists->setItemWidgetType(ITEM_WIDGET_TYPE_TRAY);
    m_connectionLists->showConnectionLists(ConnectionSettings::Wired);
    m_connectionLists->showWiredStatusIcon();
    m_connectionLists->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

//网线插入后，wiredDevice state 还处在不可用状态,因此无法立即显示出可用连接
/**
 * 暂时不使用CarrierChanged信号处理网线插拔情况，使用更改NetworkManager的配置方式处理。
 * 1、如果不更改配置，网线改变时只有CarrierChanged信号，NetworkManager不会发出其他信号，例如设备状态改变信号
 * 不会断开网络，需要自己处理流程
 * 2、如果更改配置，网线改变时，NetworkManager会自动断开已激活的网络，发出的信号较全。
 */
void WiredTrayWidget::handleCarrierChanged(bool plugged)
{
    KLOG_DEBUG() << "CarrierChanged plugged: " << plugged;
}

void WiredTrayWidget::initUnavailableWidget()
{
    m_unavailableWidget = new QWidget();
    QLabel *icon = new QLabel(m_unavailableWidget);
    QLabel *text = new QLabel(m_unavailableWidget);
    QHBoxLayout *hLayout = new QHBoxLayout(m_unavailableWidget);

    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-disconnected.svg");
    icon->setPixmap(pixmap);
    text->setText(tr("Wired network unavailable"));
    hLayout->addWidget(icon);
    hLayout->addWidget(text);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(10, 0, 0, 0);
    hLayout->addStretch();
}

void WiredTrayWidget::handleStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    KLOG_DEBUG() << "---------newstate:" << newstate;
    KLOG_DEBUG() << "---------oldstate:" << oldstate;
    KLOG_DEBUG() << "---------reason:" << reason;

    //对应拔出网线后再插入网线的情况
    if ((oldstate == Device::State::Unavailable) &&
        ((newstate != Device::State::Unmanaged) && (newstate != Device::State::Unmanaged)) &&
        (reason == Device::StateChangeReason::CarrierReason))
    {
        // if (!m_connectionLists.isNull())
        // {
        //     m_verticalLayout->removeWidget(m_unavailableWidget);
        //     m_unavailableWidget->deleteLater();

        //     m_connectionLists = new ConnectionLists(this);
        //     m_verticalLayout->addWidget(m_connectionLists);
        //     showWiredConnectionLists();
        //     KLOG_DEBUG() << "---------------available wired connnection";
        // }

        if (!m_connectionLists->isVisible())
        {
            m_connectionLists->setVisible(true);
            m_unavailableWidget->setVisible(false);
        }
    }

    //对应拔出网线
    if ((newstate == Device::State::Unavailable) && (reason == Device::StateChangeReason::CarrierReason))
    {
        // if (m_unavailableWidget == nullptr)
        // {
        //     m_verticalLayout->removeWidget(m_connectionLists);
        //     m_connectionLists->deleteLater();

        //     initUnavailableWidget();
        //     m_verticalLayout->addWidget(m_unavailableWidget);
        // }

        if (!m_unavailableWidget->isVisible())
        {
            m_connectionLists->setVisible(false);
            m_unavailableWidget->setVisible(true);
        }
    }

    // XXX:此处是对网线插拔的冗余操作，
    //因为偶现过一次，StateChanged信号丢失Device::Unavailable的情况
    if (!m_wiredDevice.isNull())
    {
        if (m_wiredDevice->carrier())
        {
            if (!m_connectionLists->isVisible())
            {
                m_connectionLists->setVisible(true);
                m_unavailableWidget->setVisible(false);
            }
        }
        else
        {
            if (!m_unavailableWidget->isVisible())
            {
                m_connectionLists->setVisible(false);
                m_unavailableWidget->setVisible(true);
            }
        }
    }
}

void WiredTrayWidget::handleRequestActivateConnection(const NetworkConnectionInfo &connectionInfo)
{
    QString devicePath = "";
    QString connectionParameter = "";
    QString connectionPath = connectionInfo.connectionPath;

    // m_devicePath 可以未空，即不指定设备
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, m_devicePath, connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        // 此处处理进入激活流程失败的原因，并不涉及流程中某个具体阶段失败的原因
        KLOG_ERROR() << "activate connection failed:" << reply.error();
        StatusNotification::connectitonFailedNotify(connectionPath);
    }
    else
    {
        KLOG_DEBUG() << "reply:" << reply.reply();
        QString activatedPath = reply.value().path();
    }
}

void WiredTrayWidget::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection = findConnection(path);
    if ((connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired) && (!connection->name().isEmpty()))
    {
        m_connectionLists->addConnectionToLists(connection, m_devicePath);
    }
}

void WiredTrayWidget::handleNotifierConnectionRemoved(const QString &path)
{
    if (!m_connectionLists.isNull())
        m_connectionLists->removeConnectionFromLists(path);
}

// XXX:可以优化
void WiredTrayWidget::handleStateDeactivated(const QString &activatedPath)
{
    if (!m_connectionLists.isNull())
        m_connectionLists->handleActiveStateDeactivated(activatedPath);
}

void WiredTrayWidget::handleStateActivated(const QString &activatedPath)
{
    KLOG_DEBUG() << "Wired  handleStateActivated";
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedPath);
    QStringList deviceList = activeConnection->devices();
    if (deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wired))
    {
        m_connectionLists->updateItemActivatedStatus(activatedPath);
        auto item = m_connectionLists->findItemByActivatedPath(activatedPath);
        if (item != nullptr)
        {
            NetworkConnectionInfo connectionInfo = item->data(Qt::UserRole).value<NetworkConnectionInfo>();
            StatusNotification::ActiveConnectionActivatedNotify(connectionInfo);
        }
    }
}

void WiredTrayWidget::handleActiveConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "handleActiveConnectionAdded path:" << path;
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection.isNull())
        return;
    QStringList deviceList = activatedConnection->devices();
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired && deviceList.contains(m_devicePath))
    {
        QString uuid = activatedConnection->uuid();
        QListWidgetItem *activeItem = m_connectionLists->findItemByUuid(uuid);
        if (activeItem != nullptr)
            m_connectionLists->updateItemActivatedPath(activeItem, path);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredTrayWidget::handleActiveConnectionStateChanged, Qt::UniqueConnection);
    }
}

//需要做判断
void WiredTrayWidget::handleStateActivating(const QString &activatedPath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activatedPath);
    if (activatedConnection.isNull())
        return;
    QStringList deviceList = activatedConnection->devices();
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wired && deviceList.contains(m_devicePath))
    {
        auto item = m_connectionLists->findItemByActivatedPath(activatedPath);
        if (item != nullptr)
            m_connectionLists->updateItemActivatingStatus(item);
        m_connectionLists->update();
    }
}

void WiredTrayWidget::handleActiveConnectionRemoved(const QString &path)
{
    if (!m_connectionLists.isNull())
        m_connectionLists->handleActiveStateDeactivated(path);
}

void WiredTrayWidget::handleRequestDisconnect(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
    else
        KLOG_INFO() << "deactivateConnection:" << reply.reply();
}

void WiredTrayWidget::handleRequestCancel(const QString &activatedConnectionPath)
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activatedConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_INFO() << "Disconnect failed:" << reply.error();
    else
        KLOG_INFO() << "deactivateConnection:" << reply.reply();
}
