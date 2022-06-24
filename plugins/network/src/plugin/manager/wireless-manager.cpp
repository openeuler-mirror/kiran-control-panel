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
#include "wireless-manager.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <QEventLoop>
#include <QScrollBar>
#include <QPointer>
#include "ui_wireless-manager.h"

WirelessManager::WirelessManager(const QString &devicePath,QWidget *parent) : Manager(parent), ui(new Ui::WirelessManager)
{
    ui->setupUi(this);
    m_devicePath = devicePath;
    m_devicePtr = findNetworkInterface(m_devicePath);
    m_wirelessDevice = qobject_cast<WirelessDevice *>(m_devicePtr);
    initUI();
    initConnection();
}

WirelessManager::~WirelessManager()
{
    delete ui;
}

void WirelessManager::initUI()
{
    ui->connectionShowPage->setConnectionType(ConnectionSettings::Wireless);
    ui->connectionShowPage->setDevicePath(m_devicePath);
    ui->connectionShowPage->setItemWidgetType(ITEM_WIDGET_TYPE_PLUGIN);
    ui->connectionShowPage->setTitle(tr("Wireless Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
    ui->connectionShowPage->setCreateButtonVisible(false);
    ui->connectionShowPage->showWirelessNetworkLists();
}

void WirelessManager::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        ui->wirelessSettingPage->showSettingPage();
        QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
        scrollBar->setValue(0);
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestEditConnection, [=](const QString &uuid, QString activeConnectionPath) {
        ActiveConnection::Ptr activeConnection = findActiveConnection(activeConnectionPath);
        ConnectionSettings::Ptr connectionSettings = activeConnection->connection()->settings();
        ui->wirelessSettingPage->setConnection(activeConnection->connection());
        ui->wirelessSettingPage->setConnectionSettings(connectionSettings);
        ui->wirelessSettingPage->initSettingPage();
        ui->wirelessSettingPage->showSettingPage(activeConnectionPath);
        QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
        scrollBar->setValue(0);
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    connect(ui->returnButton, &QPushButton::clicked, this, &WirelessManager::handleReturnPreviousPage);
    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        ui->wirelessSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wireless);
        handleReturnPreviousPage();
    });

    connect(ui->wirelessSettingPage, &WirelessSettingPage::returnPreviousPage, this, &WirelessManager::handleReturnPreviousPage);
    //XXX:更改信号
    connect(ui->wirelessSettingPage, &WirelessSettingPage::settingUpdated, [=]() {
        KLOG_DEBUG() << "WiredSettingPage::settingUpdated";
        handleReturnPreviousPage();
        refreshConnectionLists();
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestConnectWirelessNetwork,
            this, &WirelessManager::handleRequestConnectWirelessNetwork);

    initNotifierConnection();

    connect(m_wirelessDevice.data(), &WirelessDevice::networkDisappeared, this, &WirelessManager::handleNetworkDisappeared,Qt::QueuedConnection);
    connect(m_wirelessDevice.data(), &WirelessDevice::networkAppeared, this, &WirelessManager::handleNetworkAppeared);
}

//在已存在WirelessSetting配置的情况下，激活连接．（连接过一次后会创建WirelessSetting配置）
void WirelessManager::activateWirelessConnection(const QString &connectionPath, const QString &devicePath, const QString &accessPointPath)
{
    KLOG_DEBUG() << "activateWirelessConnection";
    if (!connectionPath.isEmpty())
    {
        QDBusPendingReply<QDBusObjectPath> reply =
            NetworkManager::activateConnection(connectionPath, devicePath, accessPointPath);

        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_DEBUG() << "activate connection failed:" << reply.error();
        }
        else
        {
            KLOG_DEBUG() << "reply.reply():" << reply.reply();
            QString activatedPath = reply.value().path();
        }
    }
}

void WirelessManager::getWirelessAvailableConnections(const QString &devicePath)
{
    Device::Ptr device = findNetworkInterface(devicePath);
    Connection::List connectionList = listConnections();
    m_wirelssConnectionMap.clear();
    for (Connection::Ptr conn : connectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::Wireless)
        {
            WirelessSetting::Ptr wirelessSetting = conn->settings()->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
            KLOG_DEBUG() << "wirelessSetting->ssid():" << wirelessSetting->ssid();
            KLOG_DEBUG() << "wirelessSetting->security():" << wirelessSetting->security();
            QString ssid  = QString(wirelessSetting->ssid());
            m_wirelssConnectionMap.insert(ssid, conn);
        }
    }
}

void WirelessManager::handleRequestConnectWirelessNetwork(const ConnectionInfo &connectionInfo)
{
    m_connectionInfo = connectionInfo;
    QString ssid = connectionInfo.wirelessInfo.ssid;
    KLOG_DEBUG() << "handleRequestConnectWirelessNetwork ssid:" << ssid;
    QString accessPointPath = connectionInfo.wirelessInfo.accessPointPath;

    getWirelessAvailableConnections(m_devicePath);
    if (m_wirelssConnectionMap.contains(ssid))
    {
        Connection::Ptr connection = m_wirelssConnectionMap.value(ssid);
        QString connectionPath = connection->path();
        activateWirelessConnection(connectionPath, m_devicePath, accessPointPath);
    }
    else
    {
        createConnectionSettings(ssid,accessPointPath);
        WirelessSecuritySetting::Ptr wirelessSecurity =
            m_connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
        WirelessSecuritySetting::KeyMgmt keyMgmt = wirelessSecurity->keyMgmt();
        if (keyMgmt != WirelessSecuritySetting::KeyMgmt::WpaNone)
        {
//            emit requestPasswordFromTray(ssid);
        }
        else
            addAndActivateWirelessConnection(m_connectionSettings);
    }
}


void WirelessManager::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    QStringList deviceList = activatedConnection->devices();
    if ((activatedConnection->type() == ConnectionSettings::ConnectionType::Wireless) && deviceList.contains(m_devicePath))
    {
        ConnectionSettings::Ptr settings = activatedConnection->connection()->settings();
        WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        QString ssid = wirelessSetting->ssid();
        QListWidgetItem *activeItem = ui->connectionShowPage->findItemBySsid(ssid);
        if(activeItem != nullptr)
        {
            //更新item信息
            ui->connectionShowPage->updateItemActivatedPath(activeItem, path);
        }
        else
        {
            //将排在最后的隐藏网络item复原
            int row = ui->connectionShowPage->count() - 1;
            auto item = ui->connectionShowPage->item(row);
            ui->connectionShowPage->itemSimpleStatus(item);
        }
        connect(activatedConnection.data(), &ActiveConnection::stateChanged,this,&WirelessManager::handleActiveConnectionStateChanged);

    }
}

//断开网络时，会自动rescan搜索无线网络
void WirelessManager::handleActiveConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->handleActiveStateDeactivated(path);
}

void WirelessManager::handleStateActivating(const QString &activatedPath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activatedPath);
    QStringList deviceList = activatedConnection->devices();
    if ((activatedConnection->type() == ConnectionSettings::ConnectionType::Wireless) && deviceList.contains(m_devicePath))
    {
        //加载等待动画
        auto item = ui->connectionShowPage->findItemByActivatedPath(activatedPath);
        if(item != nullptr)
            ui->connectionShowPage->updateItemActivatingStatus(item);
    }
}

void WirelessManager::handleStateActivated(const QString &activatedPath)
{
    KLOG_DEBUG() << "Wireless  handleStateActivated";
    ActiveConnection::Ptr activeConnection = findActiveConnection(activatedPath);
    if(activeConnection.isNull())
        return ;
    QStringList deviceList = activeConnection->devices();
    if (deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wireless))
    {
        KLOG_DEBUG() << "handleStateActivated activatedPath:" << activatedPath;
        ui->connectionShowPage->updateItemActivatedStatus(activatedPath);
        ui->connectionShowPage->sortItems();
        ui->connectionShowPage->update();
    }
}

void WirelessManager::handleStateDeactivated(const QString &deactivatedPath)
{
    ui->connectionShowPage->handleActiveStateDeactivated(deactivatedPath);
}

void WirelessManager::handleNotifierConnectionAdded(const QString &path)
{
}

void WirelessManager::handleNotifierConnectionRemoved(const QString &path)
{
    KLOG_DEBUG() << "WirelessManager::handleNotifierConnectionRemoved :" << path;
}

void WirelessManager::handleReturnPreviousPage()
{
    ui->wirelessSettingPage->clearPtr();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void WirelessManager::refreshConnectionLists()
{
//    QEventLoop eventLoop;
//    QTimer::singleShot(10000, &eventLoop, &QEventLoop::quit);
//    eventLoop.exec();
    ui->connectionShowPage->clearConnectionLists();
    ui->connectionShowPage->showWirelessNetworkLists();
    //    m_currentWirelessDevice->requestScan();
}

void WirelessManager::handleNetworkDisappeared(const QString &ssid)
{
    KLOG_DEBUG() << "handleNetworkDisappeared ssid:" << ssid;
    ui->connectionShowPage->removeWirelessNetworkFromLists(ssid);
}

void WirelessManager::handleNetworkAppeared(const QString &ssid)
{
    KLOG_DEBUG() << "handleNetworkAppeared ssid:" << ssid;
    WirelessNetwork::Ptr network = m_wirelessDevice->findNetwork(ssid);
    QString devicePath = m_wirelessDevice->uni();
    ui->connectionShowPage->addWirelessNetworkToLists(network, devicePath);
}

void WirelessManager::createConnectionSettings(const QString &ssid, const QString &accessPointPath)
{
    m_connectionSettings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wireless));
    m_connectionSettings->setId(ssid);
    m_connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

    WirelessSetting::Ptr wirelessSetting = m_connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wirelessSetting->setInitialized(true);
    wirelessSetting->setSsid(ssid.toUtf8());

    WirelessSecuritySetting::Ptr wirelessSecurity =
        m_connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
    wirelessSecurity->setInitialized(true);
    wirelessSetting->setSecurity(QStringLiteral("802-11-wireless-security"));

    Ipv4Setting::Ptr ipv4Setting = m_connectionSettings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    // 处理不同验证的情况
    //accessPointPath路径为空，对应隐藏网络情况，则默认为WpaPsk
    if(accessPointPath.isEmpty())
    {
        wirelessSetting->setHidden(true);
        WirelessSecuritySetting::KeyMgmt keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaPsk;
        wirelessSecurity->setKeyMgmt(keyMgmt);
    }
    else
    {
        AccessPoint::Ptr accessPoint = m_wirelessDevice->findAccessPoint(accessPointPath);
        AccessPoint::Capabilities capabilities = accessPoint->capabilities();
        AccessPoint::WpaFlags wpaFlags = accessPoint->wpaFlags();
        AccessPoint::WpaFlags rsnFlags = accessPoint->rsnFlags();

        WirelessSecuritySetting::KeyMgmt keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaNone;

        if (capabilities.testFlag(AccessPoint::Capability::Privacy) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
        {
            keyMgmt = WirelessSecuritySetting::KeyMgmt::Wep;
        }

        if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk))
        {
            keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaPsk;
        }

        if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
        {
            keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaEap;
        }
        wirelessSecurity->setKeyMgmt(keyMgmt);
    }
}

void WirelessManager::addAndActivateWirelessConnection(ConnectionSettings::Ptr connectionSettings)
{
    KLOG_DEBUG() << "addAndActivateWirelessConnection";
    const QString ssid = m_connectionInfo.wirelessInfo.ssid;
    const QString accessPointPath = m_connectionInfo.wirelessInfo.accessPointPath;
    KLOG_DEBUG() << "accessPointPath" << accessPointPath;

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply =
        NetworkManager::addAndActivateConnection(connectionSettings->toMap(), m_devicePath, accessPointPath);

    reply.waitForFinished();
    if (reply.isError())
        KLOG_DEBUG() << "Connection failed: " << reply.error();
}
