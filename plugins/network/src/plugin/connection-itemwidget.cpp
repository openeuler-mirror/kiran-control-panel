/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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

#include "connection-itemwidget.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <QApplication>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include "animation-loading-label.h"
#include "signal-forward.h"
#include "status-notification.h"
#include "text-input-dialog.h"
#include "utils.h"

using namespace NetworkManager;
// 使用默认析构函数，父对象被释放时，会释放子对象
ConnectionItemWidget::ConnectionItemWidget(QWidget* parent) : KiranFrame(parent)
{
    initUI();
}

ConnectionItemWidget::ConnectionItemWidget(NetworkConnectionInfo connectionInfo, QWidget* parent)
    : KiranFrame(parent),
      m_connectionInfo(connectionInfo)
{
    initUI();
    if (!m_connectionInfo.connectionPath.isEmpty())
    {
        m_connection = findConnection(m_connectionInfo.connectionPath);
        connect(m_connection.data(), &Connection::updated, this, &ConnectionItemWidget::updateConnection, Qt::UniqueConnection);
    }
}

void ConnectionItemWidget::initUI()
{
    setFixedHeight(36);
    initPluginItemWidget();
    setDrawBroder(false);
    setAttribute(Qt::WA_Hover);
    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &ConnectionItemWidget::handleThemeChanged);
    // TODO:obsolete
    connect(m_editButton, &QPushButton::clicked, this, &ConnectionItemWidget::editButtonClicked);

    connect(m_editButton, &QPushButton::clicked, this, &ConnectionItemWidget::editConnection);
    connect(m_disconnectAction, &QAction::triggered, this, &ConnectionItemWidget::disconnectConnection);
}

void ConnectionItemWidget::initPluginItemWidget()
{
    m_connectionTypeIcon = new QLabel(this);
    m_connectionName = new KiranLabel(this);
    m_horizonLayout = new QHBoxLayout(this);
    m_editButton = new QPushButton(this);
    m_activatedLabel = new AnimationLoadingLabel(this);
    m_moreOptions = new QPushButton(this);

    m_menu = new QMenu(this);
    m_disconnectAction = new QAction(tr("disconnect"), this);
    m_menu->addAction(m_disconnectAction);

    m_connectionName->setElideMode(Qt::TextElideMode::ElideRight);
    m_connectionTypeIcon->setVisible(false);

    m_editButton->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/details-info.svg"));
    m_editButton->setIconSize(QSize(16, 16));
    m_editButton->setFixedSize(30, 36);
    m_editButton->setFlat(true);

    m_moreOptions->setMenu(m_menu);
    m_moreOptions->setIcon(QIcon(":/kcp-network-images/more-options.svg"));
    m_moreOptions->setIconSize(QSize(16, 16));
    m_moreOptions->setFixedSize(30, 36);
    m_moreOptions->setFlat(true);
    m_moreOptions->setStyleSheet("QPushButton::menu-indicator{image:none}");

    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_connectionTypeIcon);
    m_horizonLayout->addWidget(m_connectionName);
    m_horizonLayout->addStretch();

    m_horizonEditAndMoreOptions = new QHBoxLayout();
    m_horizonEditAndMoreOptions->addWidget(m_editButton);
    m_horizonEditAndMoreOptions->addWidget(m_moreOptions);
    m_horizonEditAndMoreOptions->setMargin(0);
    m_horizonEditAndMoreOptions->setSpacing(0);

    m_horizonLayout->addLayout(m_horizonEditAndMoreOptions);

    m_horizonLayout->setMargin(0);
    m_horizonLayout->setSpacing(12);

    this->setLayout(m_horizonLayout);
    // this->setContentsMargins(10, 0, 3, 0);

    if (type() == ConnectionSettings::Wireless)
    {
        setEditButtonVisible(false);
        setMoreOptionsVisible(false);
        m_ignoreAction = new QAction(tr("ignore"), this);
        m_menu->addAction(m_ignoreAction);
        connect(m_ignoreAction, &QAction::triggered, this, &ConnectionItemWidget::ignoreWirelessNetwork);
    }
    else
    {
        m_removeAction = new QAction(tr("remove"), this);
        m_menu->addAction(m_removeAction);
        connect(m_removeAction, &QAction::triggered, this, &ConnectionItemWidget::removeConnection);
    }
}

// TODO:名称过长进行缩略
void ConnectionItemWidget::setName(const QString& name)
{
    QString nameStr = name;
    //    QFontMetricsF fontMetricsF(m_connectionName->font());
    //    if(fontMetricsF.width(nameStr) > m_connectionName->width())
    //    {
    //        nameStr = fontMetricsF.elidedText(nameStr,Qt::ElideRight,m_connectionName->width());
    //    }
    //    KLOG_DEBUG() << "elidedText:" << nameStr;
    m_connectionName->setText(nameStr);
    m_editButton->setAccessibleName(QString("ButtonEditConnectionName::%1").arg(nameStr));
}

// TODO:其他状态信息的显示，以及优化
void ConnectionItemWidget::activatedStatus()
{
    QPixmap pixmap(":/kcp-network-images/correct.png");
    m_activatedLabel->setPixmap(pixmap);
    m_activatedLabel->setAlignment(Qt::AlignCenter);
    m_activatedLabel->setVisible(true);
}

void ConnectionItemWidget::deactivateStatus()
{
    m_activatedLabel->setPixmap(QPixmap());
    setLoadingStatus(false);
    setActiveConnectionPath("");
}

void ConnectionItemWidget::setLoadingStatus(bool isLoading)
{
    m_activatedLabel->setLoadingStatus(isLoading);
}

void ConnectionItemWidget::setLabelVisible(bool isVisible)
{
    m_activatedLabel->setVisible(isVisible);
}

void ConnectionItemWidget::setWirelessStatusIcon(bool security, int signal)
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
    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

void ConnectionItemWidget::setWiredStatusIcon()
{
    // TODO:图标跟随网络状态改变
    QString svgPath = ":/kcp-network-images/wired-connection.svg";
    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

void ConnectionItemWidget::setEditButtonVisible(bool isVisible)
{
    m_editButton->setVisible(isVisible);
}

void ConnectionItemWidget::setOtherNetworkIcon()
{
    QString svgPath = ":/kcp-network-images/wireless-other-network.svg";
    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

void ConnectionItemWidget::setMoreOptionsVisible(bool isVisible)
{
    m_moreOptions->setVisible(isVisible);
}

void ConnectionItemWidget::setActiveStatus(NetworkManager::ActiveConnection::State state)
{
    switch (state)
    {
    case NetworkManager::ActiveConnection::State::Unknown:

        break;
    case NetworkManager::ActiveConnection::State::Activating:
        setLoadingStatus(true);
        setLabelVisible(true);
        break;
    case NetworkManager::ActiveConnection::State::Activated:
        setLoadingStatus(false);
        activatedStatus();
        setEditButtonVisible(true);
        setMoreOptionsVisible(true);
        break;
    case NetworkManager::ActiveConnection::State::Deactivating:
        break;
    case NetworkManager::ActiveConnection::State::Deactivated:
        deactivateStatus();
        break;
    default:
        break;
    }
}

void ConnectionItemWidget::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    setActiveStatus(state);
    if (state == NetworkManager::ActiveConnection::State::Deactivated)
    {
        // 赋值为空，清空已激活路径
        setActiveConnectionPath("");
        if (type() == ConnectionSettings::Wireless)
        {
            setEditButtonVisible(false);
            setMoreOptionsVisible(false);
        }
    }
}

void ConnectionItemWidget::activateConnection(const QString& connectionPath, const QString& connectionParameter)
{
    if (!m_connectionInfo.activeConnectionPath.isEmpty())
    {
        return;
    }

    KLOG_DEBUG() << "Will activate the device:" << m_connectionInfo.devicePath;
    QString connectionUni = connectionPath.isEmpty() ? m_connectionInfo.connectionPath : connectionPath;
    QString devicePath = m_connectionInfo.devicePath;

    if (!devicePath.isEmpty())
    {
        Device::Ptr device = NetworkManager::findNetworkInterface(devicePath);
        auto devicestate = device->state();
        KLOG_DEBUG() << "current device state:" << devicestate;
        if (devicestate == Device::Unavailable)
        {
            StatusNotification::connectitonFailedNotifyByReason(tr("The current device:%1 is not available").arg(device->interfaceName()));
            return;
        }
    }

    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionUni, devicePath, connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        // 此处处理进入激活流程失败的原因，并不涉及流程中某个具体阶段失败的原因
        KLOG_ERROR() << "activate connection failed:" << reply.error();
        QString errorMessage = reply.error().message();
        if (errorMessage.contains("device has no carrier"))
        {
            StatusNotification::connectitonFailedNotifyByReason(tr("The carrier is pulled out"));
        }
        else
        {
            StatusNotification::connectitonFailedNotify(connectionUni);
        }
    }
    else
    {
        KLOG_DEBUG() << "activateConnection reply:" << reply.reply();
    }
}

void ConnectionItemWidget::activateHiddenNetwork(const QString& ssid)
{
    m_connectionInfo.wirelessInfo.ssid = ssid;
    QString devicePath = m_connectionInfo.devicePath;
    auto device = findNetworkInterface(devicePath);
    WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice*>(device);
    // 若要连接的隐藏网络已经被显式探测到了，则返回
    if (wirelessDevice->findNetwork(ssid) != nullptr)
    {
        KLOG_DEBUG() << "Hidden networks have been explicitly detected,return";
        StatusNotification::connectitonHiddenNetworkFailedNotify(ssid);
        return;
    }
    /** Note:连接隐藏网络时不指定AccessPointPath*/
    QString accessPointPath = "";
    Connection::Ptr connection = NetworkUtils::getAvailableConnectionBySsid(devicePath, ssid);
    if (!connection.isNull())
    {
        activateConnection(connection->path(), accessPointPath);
    }
    else
    {
        m_connnectionSettings = NetworkUtils::createWirelessConnectionSettings(ssid, devicePath, accessPointPath);
        requireInputPassword();
    }
}

void ConnectionItemWidget::updateConnection()
{
    Connection::Ptr connection = findConnection(connectionPath());

    if (connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
    {
        WiredSetting::Ptr wiredSetting = connection->settings()->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
        QString mac = wiredSetting->macAddress().toHex(':').toUpper();
        QString devicePath = m_connectionInfo.devicePath;
        auto device = findNetworkInterface(devicePath);
        WiredDevice::Ptr wiredDevice = qobject_cast<WiredDevice*>(device);
        QString hardwareAddress = wiredDevice->permanentHardwareAddress();
        // 检查绑定的设备信息是否改变
        if (!mac.isEmpty() &&
            (mac != hardwareAddress))
        {
            KLOG_INFO(qLcNetwork) << "the binding device MAC has changed";
            SignalForward::instance()->connectionMacChanged(connectionPath(), mac);
            return;
        }
    }

    setName(connection->name());
}

void ConnectionItemWidget::disconnectConnection()
{
    if (activeConnectionPath().isEmpty())
    {
        return;
    }

    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activeConnectionPath());
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_INFO() << "Disconnect failed:" << reply.error();
    }
    else
    {
        KLOG_INFO() << "DeactivateConnection reply:" << reply.reply();
    }
}

void ConnectionItemWidget::removeConnection()
{
    QString tip = QString(tr("Are you sure you want to delete the connection %1")).arg(m_connection->name());
    KiranMessageBox::KiranStandardButton btn = KiranMessageBox::message(this, tr("Warning"),
                                                                        tip,
                                                                        KiranMessageBox::Yes | KiranMessageBox::No);
    if (btn != KiranMessageBox::Yes)
    {
        return;
    }
    QString connectionName = m_connection->name();
    QDBusPendingReply<> reply = m_connection->remove();
    reply.waitForFinished();
    StatusNotification::connectionDeleteNotify(connectionName);
    if (reply.isError())
    {
        KLOG_INFO() << "Delete the connection failed:" << reply.error();
    }
    else
    {
        KLOG_INFO() << "remove the connection :" << this->connectionPath();
    }
}

void ConnectionItemWidget::setPskAndActivateWirelessConnection(const QString& password)
{
    WirelessSecuritySetting::Ptr wirelessSecurity =
        m_connnectionSettings->setting(Setting::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();
    wirelessSecurity->setPsk(password);
    wirelessSecurity->setPskFlags(Setting::SecretFlagType::None);  // default: Save password for all users
    wirelessSecurity->setInitialized(true);

    addAndActivateWirelessConnection(m_connnectionSettings);
}

// NOTE:ignore无线网络会删除配置，功能目前与remove重合
void ConnectionItemWidget::ignoreWirelessNetwork()
{
    QString devicePath = m_connectionInfo.devicePath;
    Connection::Ptr connection = NetworkUtils::getAvailableConnectionBySsid(devicePath, ssid());
    if (connection.isNull())
    {
        return;
    }

    QDBusPendingReply<> reply = connection->remove();
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_INFO() << "remove the connection failed:" << reply.error();
    }
    else
    {
        KLOG_INFO() << "ignore the wireless network:" << ssid();
    }
}

void ConnectionItemWidget::requireInputPassword()
{
    TextInputDialog inputDialog;
    inputDialog.setTitle(tr("Tips"));
    QString tips = QString(tr("Password required to connect to %1.")).arg(ssid());
    inputDialog.setText(tips);
    inputDialog.setlineEditEchoMode(QLineEdit::Password);
    connect(&inputDialog, &TextInputDialog::password, this, &ConnectionItemWidget::setPskAndActivateWirelessConnection);

    inputDialog.exec();
}

void ConnectionItemWidget::requireHiddenNetworkName()
{
    KLOG_DEBUG() << "connect hidden network";
    TextInputDialog ssidInputDialog;
    ssidInputDialog.setTitle(tr("Tips"));
    QString tips = QString(tr("Please input a network name"));
    ssidInputDialog.setText(tips);
    connect(&ssidInputDialog, &TextInputDialog::ssid, this, &ConnectionItemWidget::activateHiddenNetwork);
    ssidInputDialog.exec();
}

void ConnectionItemWidget::addAndActivateWirelessConnection(NetworkManager::ConnectionSettings::Ptr connectionSettings)
{
    const QString accessPointPath = m_connectionInfo.wirelessInfo.accessPointPath;
    const QString devicePath = m_connectionInfo.devicePath;
    KLOG_DEBUG() << "accessPointPath" << accessPointPath;

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply =
        NetworkManager::addAndActivateConnection(connectionSettings->toMap(), devicePath, accessPointPath);

    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_DEBUG() << "Connection failed: " << reply.error();
        StatusNotification::connectitonFailedNotifyByName(ssid());
    }
}

void ConnectionItemWidget::activateWirelessNetwork()
{
    KLOG_DEBUG() << "Activate Selected Wireless Network:" << ssid();
    QString accessPointPath = m_connectionInfo.wirelessInfo.accessPointPath;

    // 连接隐藏网络
    if (ssid().isEmpty() && accessPointPath.isEmpty())
    {
        requireHiddenNetworkName();
        return;
    }

    QString devicePath = m_connectionInfo.devicePath;
    Connection::Ptr connection = NetworkUtils::getAvailableConnectionBySsid(devicePath, ssid());

    // 在已存在WirelessSetting配置的情况下，激活连接．（连接过一次后会创建WirelessSetting配置）
    if (!connection.isNull())
    {
        activateConnection(connection->path(), accessPointPath);
        return;
    }

    m_connnectionSettings = NetworkUtils::createWirelessConnectionSettings(ssid(), devicePath, accessPointPath);
    WirelessSecuritySetting::Ptr wirelessSecurity =
        m_connnectionSettings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
    WirelessSecuritySetting::KeyMgmt keyMgmt = wirelessSecurity->keyMgmt();
    if (keyMgmt != WirelessSecuritySetting::KeyMgmt::WpaNone)
    {
        requireInputPassword();
        return;
    }
    addAndActivateWirelessConnection(m_connnectionSettings);
}

void ConnectionItemWidget::handleThemeChanged(Kiran::PaletteType paletteType)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QPixmap pixmap = m_connectionTypeIcon->pixmap(Qt::ReturnByValue);
    QImage image = pixmap.toImage();
#else
    QImage image = m_connectionTypeIcon->pixmap()->toImage();
#endif
    image.invertPixels(QImage::InvertRgb);
    m_connectionTypeIcon->setPixmap(QPixmap::fromImage(image));
    m_editButton->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/details-info.svg"));
}

void ConnectionItemWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
        if (type() == ConnectionSettings::Wireless)
        {
            activateWirelessNetwork();
        }
        else if (type() == ConnectionSettings::Wired)
        {
            activateConnection();
        }
    }
    QWidget::mousePressEvent(event);
}

void ConnectionItemWidget::editConnection()
{
    SignalForward::instance()->editConnection(m_connectionInfo);
}