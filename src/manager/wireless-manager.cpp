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
WirelessManager::WirelessManager(QWidget *parent) : Manager(parent), ui(new Ui::WirelessManager)
{
    ui->setupUi(this);
    initDevice();
    initUI();
    initConnection();
}

WirelessManager::~WirelessManager()
{
    delete ui;
}

void WirelessManager::initDevice()
{
    getDeviceList(Device::Wifi);
    if (m_deviceList.count() == 0)
    {
        //处理没有设备时的情况
    }
    else if (m_deviceList.count() == 1)
    {
        //只有一个设备时
        m_currentWirelessDevice = qobject_cast<WirelessDevice *>(m_deviceList.at(0));
    }
    else
    {
        //存在多个设备时
    }
}

void WirelessManager::initUI()
{
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

    //检测到新设备
    connect(notifier(), &Notifier::deviceAdded, [=](const QString &uni) {
        KLOG_DEBUG() << "deviceAdded:" << uni;
    });

    connect(notifier(), &Notifier::deviceRemoved, [=](const QString &uni) {
        KLOG_DEBUG() << "deviceRemoved:" << uni;
    });

    initNotifierConnection();

    connect(m_currentWirelessDevice.data(), &WirelessDevice::networkDisappeared, this, &WirelessManager::handleNetworkDisappeared,Qt::QueuedConnection);
    connect(m_currentWirelessDevice.data(), &WirelessDevice::networkAppeared, this, &WirelessManager::handleNetworkAppeared);
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
    connectionInfo.connectionPath;
    QString ssid = connectionInfo.wirelessInfo.ssid;
    KLOG_DEBUG() << "handleRequestConnectWirelessNetwork ssid:" << ssid;
    QString accessPointPath = connectionInfo.wirelessInfo.accessPointPath;
    QString devicePath = connectionInfo.devicePath;
    getWirelessAvailableConnections(devicePath);
    if (m_wirelssConnectionMap.contains(ssid))
    {
        Connection::Ptr connection = m_wirelssConnectionMap.value(ssid);
        QString connectionPath = connection->path();
        KLOG_DEBUG() << "m_wirelssConnectionMap.value(ssid) connectionPath:" << connectionPath;
        activateWirelessConnection(connectionPath, devicePath, accessPointPath);
    }
    else
        addAndActivateWirelessConnection(ssid, devicePath, accessPointPath);
}

//不存在WirelessSetting配置的情况下，连接无线网络
void WirelessManager::addAndActivateWirelessConnection(const QString &ssid, const QString &devicePath, const QString &accessPointPath)
{
    KLOG_DEBUG() << "addAndActivateWirelessConnection";
    Device::Ptr device = findNetworkInterface(devicePath);
    WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice *>(device);
    AccessPoint::Ptr accessPoint = wirelessDevice->findAccessPoint(accessPointPath);
    KLOG_DEBUG() << "accessPoint.isNull:" << accessPoint.isNull();
    KLOG_DEBUG() << "accessPoint->ssid():" << accessPoint->ssid();

    //Note:WirelessDevice在rescan搜索网络时，使用以下函数会失效，错误原因：没有及时更新ScanList,导致搜索不到network
    /*
    WirelessNetwork::List wirelessNetworkList = wirelessDevice->networks();
    for (WirelessNetwork::Ptr network : wirelessNetworkList)
    {
        KLOG_DEBUG() << "network->ssid:" << network->ssid();
    }
    WirelessNetwork::Ptr network = wirelessDevice->findNetwork(ssid);
    KLOG_DEBUG() << "network.isNull():" << network.isNull();
    KLOG_DEBUG() << "network:" << network;
    AccessPoint::Ptr referenceAP = network->referenceAccessPoint();
    */

    QScopedPointer<ConnectionSettings> settings(new ConnectionSettings(ConnectionSettings::Wireless));
    settings->setId(ssid);
    QString uuid = settings->createNewUuid();
    settings->setUuid(uuid);

    WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wirelessSetting->setSsid(ssid.toUtf8());

    Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    //处理不需要验证的情况
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
        //?
        //        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::Ieee8021x;
    }

    WirelessSecuritySetting::Ptr wirelessSecurity =
        settings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
    wirelessSecurity->setKeyMgmt(keyMgmt);

    if (keyMgmt != WirelessSecuritySetting::KeyMgmt::WpaNone)
    {
        setWirelessSecuritySetting(wirelessSecurity);
        //暂不清楚效果
        //        wirelessSetting->setSecurity("802-11-wireless-security");
    }

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply =
        NetworkManager::addAndActivateConnection(settings->toMap(), wirelessDevice->uni(), accessPointPath);

    reply.waitForFinished();

    if (reply.isValid())
    {
    }
    else
    {
        KLOG_DEBUG() << "Connection failed: " << reply.error();
    }
}

void WirelessManager::setWirelessSetting(const WirelessSetting::Ptr &wirelessSetting)
{
}

void WirelessManager::setWirelessSecuritySetting(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting)
{
    wirelessSecuritySetting->setPsk("12345678");
    wirelessSecuritySetting->setPskFlags(Setting::SecretFlagType::None);  //default: Save password for all users
    wirelessSecuritySetting->setInitialized(true);
}

void WirelessManager::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Wireless)
    {
        ConnectionSettings::Ptr settings = activatedConnection->connection()->settings();
        WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        QString ssid = wirelessSetting->ssid();
        int row = ui->connectionShowPage->findItemBySsid(ssid);
        ui->connectionShowPage->setCurrentActiveItem(row);
        connect(activatedConnection.data(), &ActiveConnection::stateChanged,this,&WirelessManager::handleActiveConnectionStateChanged);
        //加载等待动画
        ui->connectionShowPage->connectionItemLoadingAnimation();
    }
}

//断开网络时，会自动rescan搜索无线网络
void WirelessManager::handleActiveConnectionRemoved(const QString &path)
{
    KLOG_DEBUG() << "WirelessManager::handleActiveConnectionRemoved:" << path;
}

void WirelessManager::handleStateActivated(const QString &activatedPath)
{
    ui->connectionShowPage->connectionStateNotify(ActiveConnection::Activated,activatedPath);
    ui->connectionShowPage->updateActivatedConnectionInfo(activatedPath);
    ui->connectionShowPage->update();
}

void WirelessManager::handleStateDeactivated(const QString &deactivatedPath)
{
    KLOG_DEBUG() << "WirelessManager::handleActiveConnectionStateChanged";
    ui->connectionShowPage->connectionStateNotify(ActiveConnection::Deactivated,deactivatedPath);
    ui->connectionShowPage->clearDeactivatedConnectionInfo(deactivatedPath);
    ui->connectionShowPage->update();
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
    QEventLoop eventLoop;
    QTimer::singleShot(10000, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
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
    WirelessNetwork::Ptr network = m_currentWirelessDevice->findNetwork(ssid);
    QString devicePath = m_currentWirelessDevice->uni();
    ui->connectionShowPage->addWirelessNetworkToLists(network, devicePath);
}
