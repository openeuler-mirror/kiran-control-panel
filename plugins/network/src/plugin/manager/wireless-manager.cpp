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
#include <style-property.h>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <QEventLoop>
#include <QPointer>
#include <QScrollBar>
#include "signal-forward.h"
#include "status-notification.h"
#include "text-input-dialog.h"
#include "ui_wireless-manager.h"

using namespace NetworkManager;

WirelessManager::WirelessManager(const QString &devicePath, QWidget *parent) : Manager(parent), ui(new Ui::WirelessManager)
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
    ui->connectionShowPage->init(ConnectionSettings::Wireless, m_devicePath);
    ui->connectionShowPage->setTitle(tr("Wireless Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
    ui->connectionShowPage->setCreateButtonVisible(false);
    Kiran::StylePropertyHelper::setButtonType(ui->saveButton, Kiran::BUTTON_Default);

    // XXX:由于存在switchButton，所以特别修改一下topMargin
    //    ui->connectionShowPage->setContentsMargins(0,-12,0,0);
}

void WirelessManager::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::creatConnection, this, &WirelessManager::handleCreatConnection);
    connect(ui->connectionShowPage, &ConnectionShowPage::editConnection, this, &WirelessManager::handleEditConnection);

    connect(ui->returnButton, &QPushButton::clicked, this, &WirelessManager::handleReturnPreviousPage);
    connect(ui->saveButton, &QPushButton::clicked, this, [this]()
            {
                ui->wirelessSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wireless);
                handleReturnPreviousPage(); });

    connect(ui->wirelessSettingPage, &WirelessSettingPage::returnPreviousPage, this, &WirelessManager::handleReturnPreviousPage);
    // XXX:更改信号
    connect(ui->wirelessSettingPage, &WirelessSettingPage::settingUpdated, this, [this]()
            {
                KLOG_DEBUG() << "WiredSettingPage::settingUpdated";
                handleReturnPreviousPage();
                refreshConnectionLists(); });

    connect(ui->connectionShowPage, &ConnectionShowPage::activateSelectedWirelessNetwork, this, &WirelessManager::handleActivateSelectedWirelessNetwork);
    connect(ui->connectionShowPage, &ConnectionShowPage::sendSsidToWireless, this, &WirelessManager::handleActivateHiddenNetwork);

    connect(m_wirelessDevice.data(), &WirelessDevice::networkDisappeared, this, &WirelessManager::handleNetworkDisappeared, Qt::QueuedConnection);
    connect(m_wirelessDevice.data(), &WirelessDevice::networkAppeared, this, &WirelessManager::handleNetworkAppeared);

    // Note:插件与托盘都对该设备的信号进行了连接，容易干扰重复，因此，插件暂未实现该函数
    connect(m_devicePtr.data(), &Device::stateChanged, this, &WirelessManager::handleDeviceStateChanged, Qt::UniqueConnection);

    connect(m_signalForward, &SignalForward::wirelessConnectionAdded, this, &WirelessManager::handleNotifierConnectionAdded);
    connect(m_signalForward, &SignalForward::wirelessActiveConnectionAdded, this, &WirelessManager::handleActiveConnectionAdded);
}

//在已存在WirelessSetting配置的情况下，激活连接．（连接过一次后会创建WirelessSetting配置）
void WirelessManager::activateWirelessConnection(const QString &connectionPath, const QString &devicePath, const QString &accessPointPath)
{
    KLOG_DEBUG() << "connectionPath:" << connectionPath;
    KLOG_DEBUG() << "devicePath:" << devicePath;
    KLOG_DEBUG() << "accessPointPath:" << accessPointPath;
    if (!connectionPath.isEmpty())
    {
        QDBusPendingReply<QDBusObjectPath> reply =
            NetworkManager::activateConnection(connectionPath, devicePath, accessPointPath);

        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_ERROR() << "activate connection failed:" << reply.error();
            StatusNotification::connectitonFailedNotify(connectionPath);
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
    Connection::List availableConnectionList = m_devicePtr->availableConnections();
    m_wirelssConnectionMap.clear();
    for (Connection::Ptr conn : availableConnectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::Wireless)
        {
            WirelessSetting::Ptr wirelessSetting = conn->settings()->setting(Setting::SettingType::Wireless).dynamicCast<WirelessSetting>();
            QString ssid = QString(wirelessSetting->ssid());
            KLOG_DEBUG() << "Wireless Available Connections wirelessSetting->ssid():" << ssid;
            m_wirelssConnectionMap.insert(ssid, conn);
        }
    }
}

void WirelessManager::handleCreatConnection()
{
    ui->wirelessSettingPage->showSettingPage();
    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void WirelessManager::handleEditConnection(const QString &uuid, QString activeConnectionPath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activeConnectionPath);
    ConnectionSettings::Ptr connectionSettings = activeConnection->connection()->settings();
    ui->wirelessSettingPage->setConnection(activeConnection->connection());
    ui->wirelessSettingPage->setConnectionSettings(connectionSettings);
    ui->wirelessSettingPage->initSettingPage();
    ui->wirelessSettingPage->showSettingPage(activeConnectionPath);
    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void WirelessManager::handleActivateSelectedWirelessNetwork(const NetworkConnectionInfo &connectionInfo)
{
    m_connectionInfo = connectionInfo;
    QString ssid = connectionInfo.wirelessInfo.ssid;
    KLOG_DEBUG() << "Activate Selected Wireless Network:" << ssid;
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
        createConnectionSettings(ssid, accessPointPath);
        WirelessSecuritySetting::Ptr wirelessSecurity =
            m_connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
        WirelessSecuritySetting::KeyMgmt keyMgmt = wirelessSecurity->keyMgmt();
        if (keyMgmt != WirelessSecuritySetting::KeyMgmt::WpaNone)
        {
            requireInputPassword(ssid);
        }
        else
            addAndActivateWirelessConnection(m_connectionSettings);
    }
}

// TODO:什么情况下activatedConnection会为空
//无线网络连接不存在的隐藏网络时，有时会出现 findActiveConnection 为空的情况
void WirelessManager::handleActiveConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "Active Connection Added:" << path;
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection == nullptr)
    {
        KLOG_DEBUG() << "new add activatedConnection is nullptr";
        return;
    }
    QStringList deviceList = activatedConnection->devices();
    if (deviceList.contains(m_devicePath))
    {
        ConnectionSettings::Ptr settings = activatedConnection->connection()->settings();
        WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        QString ssid = wirelessSetting->ssid();
        auto *activeItemWidget = ui->connectionShowPage->findItemWidgetBySsid(ssid);
        if (activeItemWidget != nullptr)
        {
            //更新item信息
            ui->connectionShowPage->updateItemWidgetActivePath(activeItemWidget, path);
        }
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WirelessManager::handleActiveConnectionStateChanged, Qt::UniqueConnection);
    }
}

//断开网络时，会自动触发rescan搜索无线网络
void WirelessManager::handleActiveConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->handleActiveStateDeactivated(path);
}

void WirelessManager::handleStateActivating(const QString &activePath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activePath);
    if (activatedConnection == nullptr)
    {
        //连接一个不存在的无线网络时，activatedConnection为空
        KLOG_DEBUG() << "activatedConnection == nullptr";
        return;
    }
    QStringList deviceList = activatedConnection->devices();
    if ((activatedConnection->type() == ConnectionSettings::ConnectionType::Wireless) && deviceList.contains(m_devicePath))
    {
        //加载等待动画
        ui->connectionShowPage->setItemWidgetStatus(activePath, ActiveConnection::State::Activating);
    }
}

void WirelessManager::handleStateActivated(const QString &activePath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activePath);
    if (activeConnection.isNull())
        return;
    QStringList deviceList = activeConnection->devices();
    if (deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wireless))
    {
        KLOG_DEBUG() << "handleStateActivated activatedPath:" << activePath;
        ui->connectionShowPage->setItemWidgetStatus(activePath, ActiveConnection::State::Activated);
        ui->connectionShowPage->sort();
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
    ui->connectionShowPage->clearConnectionList();
    ui->connectionShowPage->showWirelessNetworkList();
    //    m_currentWirelessDevice->requestScan();
}

void WirelessManager::handleNetworkDisappeared(const QString &ssid)
{
    ui->connectionShowPage->removeWirelessNetworkFromList(ssid);
}

void WirelessManager::handleNetworkAppeared(const QString &ssid)
{
    WirelessNetwork::Ptr network = m_wirelessDevice->findNetwork(ssid);
    QString devicePath = m_wirelessDevice->uni();
    ui->connectionShowPage->addWirelessNetwork(network, devicePath);
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
    // accessPointPath路径为空，对应隐藏网络情况，则默认为WpaPsk
    if (accessPointPath.isEmpty())
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
    const QString ssid = m_connectionInfo.wirelessInfo.ssid;
    const QString accessPointPath = m_connectionInfo.wirelessInfo.accessPointPath;
    KLOG_DEBUG() << "accessPointPath" << accessPointPath;

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply =
        NetworkManager::addAndActivateConnection(connectionSettings->toMap(), m_devicePath, accessPointPath);

    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_DEBUG() << "Connection failed: " << reply.error();
        StatusNotification::connectitonFailedNotifyByName(ssid);
    }
}

void WirelessManager::requireInputPassword(const QString &ssid)
{
    TextInputDialog inputDialog;
    inputDialog.setTitle(tr("Tips"));
    QString tips = QString(tr("Password required to connect to %1.")).arg(ssid);
    inputDialog.setText(tips);
    inputDialog.setlineEditEchoMode(QLineEdit::Password);
    connect(&inputDialog, &TextInputDialog::password, this, &WirelessManager::setSecurityPskAndActivateWirelessConnection);

    inputDialog.exec();
}

void WirelessManager::setSecurityPskAndActivateWirelessConnection(const QString &password)
{
    WirelessSecuritySetting::Ptr wirelessSecurity =
        m_connectionSettings->setting(Setting::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();

    wirelessSecurity->setPsk(password);
    wirelessSecurity->setPskFlags(Setting::SecretFlagType::None);  // default: Save password for all users
    wirelessSecurity->setInitialized(true);
    addAndActivateWirelessConnection(m_connectionSettings);
}

void WirelessManager::handleActivateHiddenNetwork(const QString &ssid)
{
    m_connectionInfo.wirelessInfo.ssid = ssid;
    //若要连接的隐藏网络已经被显式探测到了，则返回
    if (m_wirelessDevice->findNetwork(ssid) != nullptr)
    {
        KLOG_DEBUG() << "Hidden networks have been explicitly detected,return";
        StatusNotification::connectitonHiddenNetworkFailedNotify(ssid);
        return;
    }
    /** Note:连接隐藏网络时不指定AccessPointPath*/
    QString accessPointPath = "";
    getWirelessAvailableConnections(m_devicePath);
    if (m_wirelssConnectionMap.contains(ssid))
    {
        Connection::Ptr connection = m_wirelssConnectionMap.value(ssid);
        QString connectionPath = connection->path();
        activateWirelessConnection(connectionPath, m_devicePath, accessPointPath);
    }
    else
    {
        createConnectionSettings(ssid, accessPointPath);
        requireInputPassword(ssid);
    }
}

void WirelessManager::handleDeviceStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason)
{
}