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

#include "network-tray.h"
#include <qt5-log-i.h>
#include <style-palette.h>
#include <NetworkManagerQt/Settings>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QTcpSocket>
#include <QTimer>
#include <QVBoxLayout>
#include "config.h"
#include "logging-category.h"
#include "status-notification.h"
#include "status-notifier-manager.h"
#include "tray-page.h"
#include "utils.h"
#include "wired-tray-widget.h"
#include "wireless-tray-widget.h"

using namespace NetworkManager;

#define STATUS_NOTIFIER_MANAGER "org.kde.StatusNotifierManager"
#define STATUS_NOTIFIER_MANAGER_OBJECT_NAME "/StatusNotifierManager"
#define MAX_WAIT_COUNTS 10
#define MAX_HEIGHT 868

NetworkTray::NetworkTray(QWidget *parent) : KiranRoundedTrayPopup(parent),
                                            m_wiredTrayPage(nullptr),
                                            m_wirelessTrayPage(nullptr),
                                            m_unavailableWidget(nullptr)
{
    init();
}

NetworkTray::~NetworkTray()
{
}

void NetworkTray::init()
{
    m_statusNotifierManager = new StatusNotifierManagerInterface(STATUS_NOTIFIER_MANAGER, STATUS_NOTIFIER_MANAGER_OBJECT_NAME, QDBusConnection::sessionBus(), this);
    initTcpSocket();
    initUI();
    initMenu();
    initConnect();

    // XXX:现将widget移到屏幕外，防止第一次显示页面，由于没指定位置而闪现在左上角，之后统一修改页面显示逻辑
    this->move(-1000, -1000);

    // 跟踪所有设备的状态变化
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        auto type = dev->type();
        if (type == Device::Ethernet || type == Device::Wifi)
        {
            connect(dev.data(), &Device::stateChanged, this, &NetworkTray::handleDeviceStateChanged, Qt::UniqueConnection);
        }
    }
}

void NetworkTray::initUI()
{
    initTrayPage();
    initTrayIcon();
}

void NetworkTray::initConnect()
{
    connect(m_systemTray, &QSystemTrayIcon::activated, this, &NetworkTray::handleTrayClicked);

    // TODO:优化deviceAdded的逻辑
    //  需要等待一段时间，Device::List networkInterfaces() 来不及更新
    connect(notifier(), &Notifier::deviceAdded, this, [this](const QString &uni)
            {
                m_addDevicePath = uni;
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                if(device.isNull())
                {
                    KLOG_DEBUG(qLcNetwork) << "this device interface is not found";
                    return;
                }
                KLOG_DEBUG(qLcNetwork) << "device->uni():" << device->uni();
                KLOG_DEBUG(qLcNetwork) << "device->interfaceName():" << device->interfaceName();
                KLOG_DEBUG(qLcNetwork) << "device->state():" << device->state();
                KLOG_DEBUG(qLcNetwork) << "device->isValid():" << device->isValid();
                if(device->isValid())
                {
                    handleDeviceAdded(m_addDevicePath);
                }               
                else
                {
                    KLOG_INFO(qLcNetwork) << "this device interface is invalid!";
                    m_Timer.start();
                    KLOG_INFO(qLcNetwork) << "wait counts:" << m_waitCounts;
                } });

    m_Timer.setInterval(500);
    m_Timer.setSingleShot(true);
    m_waitCounts = 1;
    connect(&m_Timer, &QTimer::timeout, this, [this]()
            {
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                if(device.isNull())
                {
                    KLOG_DEBUG(qLcNetwork) << "this device interface is not found";
                    return ;
                }
                if(device->isValid())
                {
                    handleDeviceAdded(m_addDevicePath);
                    m_Timer.stop();
                }
                else
                {
                    KLOG_INFO(qLcNetwork) << "this device interface is not ready";
                    m_Timer.start();
                }
                m_waitCounts++;
                if(m_waitCounts > MAX_WAIT_COUNTS)
                {
                    KLOG_INFO(qLcNetwork) << "This device is not currently managed by NetworkManager";
                    m_Timer.stop();
                } });

    connect(notifier(), &Notifier::deviceRemoved, this, &NetworkTray::handleDeviceRemoved);

    connect(notifier(), &Notifier::statusChanged, this, &NetworkTray::handleNetworkManagerStatusChanged);

    connect(notifier(), &Notifier::primaryConnectionChanged, this, &NetworkTray::handlePrimaryConnectionChanged);

    connect(notifier(), &Notifier::wirelessEnabledChanged, this, &NetworkTray::handleWirelessEnabledChanged, Qt::UniqueConnection);

    // 无线网络如果一下消失多个网络，短时间会触发多次SizeHint变更的信号
    m_wirelessTimer.setInterval(10);
    m_wirelessTimer.setSingleShot(true);
    connect(&m_wirelessTimer, &QTimer::timeout, this, [this]()
            { handleAdjustedTraySize(m_wirelessTraySizeHint); });

    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &NetworkTray::handleThemeChanged);
}

void NetworkTray::initTrayIcon()
{
    m_systemTray = new QSystemTrayIcon();
    updateTrayIcon();
    KLOG_DEBUG(qLcNetwork) << " NetworkManager::status():" << NetworkManager::status();
    m_systemTray->show();
}

void NetworkTray::initMenu()
{
    m_menu = new QMenu(this);
    m_networkSetting = new QAction(tr("Network settings"));

    m_menu->addAction(m_networkSetting);
    getTrayGeometry();
    m_menu->sizeHint();
    //    m_menu->exec();
    //    m_menu->popup(QPoint(500,500));
    m_systemTray->setContextMenu(m_menu);
    connect(m_networkSetting, &QAction::triggered, this, &NetworkTray::handleNetworkSettingClicked);
}

// 初始化条件：设备存在且被管理
void NetworkTray::initTrayPage()
{
    KLOG_DEBUG(qLcNetwork) << "init Tray Page";
    m_wiredDeviceList = NetworkUtils::getAvailableDeviceList(Device::Ethernet);
    m_wirelessDeviceList = NetworkUtils::getAvailableDeviceList(Device::Wifi);

    int wiredCount = m_wiredDeviceList.count();
    int wirelessCount = m_wirelessDeviceList.count();

    QWidget *widget = new QWidget(this);
    widget->setContentsMargins(0, 0, 0, 0);
    m_verticalLayout = new QVBoxLayout(widget);
    if (wiredCount == 0 && wirelessCount == 0)
    {
        if (m_unavailableWidget == nullptr)
        {
            initUnavailableWidget();
            m_verticalLayout->addWidget(m_unavailableWidget);
        }
    }
    else
    {
        if (wiredCount != 0)
        {
            m_wiredTrayPage = new TrayPage(m_wiredDeviceList, this);
            m_verticalLayout->addWidget(m_wiredTrayPage);
            connect(m_wiredTrayPage, &TrayPage::sizeChanged, this, &NetworkTray::handleAdjustedTraySize, Qt::UniqueConnection);
        }
        if (wirelessCount != 0)
        {
            m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
            m_verticalLayout->addWidget(m_wirelessTrayPage);

            connect(m_wirelessTrayPage, &TrayPage::sizeChanged, this, [this](QSize sizeHint)
                    {
                        m_wirelessTraySizeHint = sizeHint;
                        m_wirelessTimer.start(); });
        }
    }
    m_verticalLayout->setMargin(0);
    m_verticalLayout->setSpacing(0);
    m_verticalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    setContentWidget(widget);
    setMaximumHeight(MAX_HEIGHT);
}

void NetworkTray::initUnavailableWidget()
{
    m_unavailableWidget = new QWidget(this);
    m_unavailableWidget->setFixedSize(QSize(240, 50));

    QLabel *icon = new QLabel(m_unavailableWidget);
    QLabel *text = new QLabel(m_unavailableWidget);
    QHBoxLayout *hLayout = new QHBoxLayout(m_unavailableWidget);

    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-disconnected.svg");
    icon->setPixmap(pixmap);
    text->setText(tr("Network unavailable"));
    hLayout->addWidget(icon);
    hLayout->addWidget(text);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(10, 0, 0, 0);
    hLayout->addStretch();
}

// Note:点击托盘显示页面的同时,让所有无线设备扫描一次网络
void NetworkTray::handleTrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        showOrHideTrayPage();
        break;
    default:
        break;
    }

    for (auto device : m_wirelessDeviceList)
    {
        WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice *>(device);
        QDBusPendingReply<> replyRequestScan = wirelessDevice->requestScan();

        replyRequestScan.waitForFinished();
        if (replyRequestScan.isError())
        {
            KLOG_DEBUG(qLcNetwork) << "wireless Device name:" << wirelessDevice->interfaceName() << " requestScan error:" << replyRequestScan.error();
        }
        else
        {
            KLOG_DEBUG(qLcNetwork) << "wireless Device name:" << wirelessDevice->interfaceName() << " requestScan reply:" << replyRequestScan.reply();
        }
    }
}

void NetworkTray::handleNetworkSettingClicked()
{
    QProcess process(this);
    QStringList arguments;
    arguments << "-c"
              << "network";
    process.startDetached("kiran-control-panel", arguments);
}

void NetworkTray::showOrHideTrayPage()
{
    if (this->isVisible())
        this->hide();
    else
    {
        setTrayPagePos();
        // XXX:托盘界面在不可见的情况，不方便去修改size和位置，暂时先显示后在调整大小和位置
        // this->setFixedSize(258, 258);
        this->show();
        QTimer::singleShot(50, this, [this]()
                           {
                                    /**
                                     * 1、当同时存在有线和无线网络托盘页面时，使用adjustSize已经能够得到较好的伸缩效果
                                     * 2、当有线或无线只有其一时，最小sizeHint height为50,但adjustSize调整的window尺寸最小为（200,100）
                                     *    此时则指定页面size大小
                                     */
                                    //TODO:需要继续优化界面伸缩
                                    if(m_wiredTrayPage && m_wirelessTrayPage)
                                        adjustSize();
                                    else
                                    {
                                        this->resize(this->sizeHint());
                                    }
                                setTrayPagePos(); });
    }
}

void NetworkTray::setTrayPagePos()
{
    int pageHeight = this->size().height();
    int pageWidth = this->size().width();

    getTrayGeometry();
    // 抵消KiranRoundedTrayPopup的margin
    int offset = 8;
    int showPosY;
    // 托盘程序在顶端
    if (m_yTray == 0)
    {
        showPosY = m_heightTray - offset;
    }
    else
    {
        // 托盘程序在底部
        showPosY = m_yTray - pageHeight + offset;
    }
    this->move(m_xTray - pageWidth / 2, showPosY);
}

void NetworkTray::getTrayGeometry()
{
    // 名称待修改
    QDBusPendingReply<QString> getGeometry = m_statusNotifierManager->GetGeometry("~04-network");
    KLOG_DEBUG(qLcNetwork) << "getGeometry.value():" << getGeometry.value();

    double height, width, x, y;
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getGeometry.value().toLatin1(), &jsonParseError);
    if (!doc.isNull() && doc.isObject() && jsonParseError.error == QJsonParseError::NoError)
    {
        QJsonObject object = doc.object();
        QStringList list = object.keys();
        height = object.value("height").toDouble();
        width = object.value("width").toDouble();
        x = object.value("x").toDouble();
        y = object.value("y").toDouble();
    }
    m_heightTray = static_cast<int>(height);
    m_widthTray = static_cast<int>(width);
    m_xTray = static_cast<int>(x);
    m_yTray = static_cast<int>(y);
}

void NetworkTray::updateTrayIcon()
{
    auto status = NetworkManager::status();
    if (status < NetworkManager::Status::Connected)
    {
        setTrayIcon(DISCONNECTED);
        return;
    }

#define SET_TRAY_ICON_AND_CHECK_CONNECTIVITY_AND_RETURN(state) \
    if (state != UNKNOWN)                                      \
    {                                                          \
        setTrayIcon(state);                                    \
        checkInternetConnectivity();                           \
        return;                                                \
    }

    NetworkState state = UNKNOWN;
    // 判断主连接类型，托盘网络图标以主连接类型为准
    // NetworkManager::primaryConnectionType() 更新不及时，暂时不用
    /**
     * NOTE:
     * 注意特殊情况，如果当网络状态为已连接，但是没有主连接，则遍历所有已激活的连接，
     * 按有线优先于无线的原则，如果存在激活的有线连接，则显示有线网络图标;其次显示无线网络图标
     * 如果既不是有线也不是无线，则显示有线网络图标
     */
    ActiveConnection::Ptr primaryActiveConnection = primaryConnection();
    if (!primaryActiveConnection.isNull())
    {
        if (primaryActiveConnection->type() == ConnectionSettings::Wireless)
        {
            state = WIRELESS_CONNECTED_BUT_NOT_ACCESS_INTERNET;
        }
        else
        {
            state = WIRED_CONNECTED_BUT_NOT_ACCESS_INTERNET;
        }
    }
    SET_TRAY_ICON_AND_CHECK_CONNECTIVITY_AND_RETURN(state);

    KLOG_INFO(qLcNetwork) << "primary active connection is null";
    ActiveConnection::List list = activeConnections();
    for (auto connection : list)
    {
        if (connection->type() == ConnectionSettings::ConnectionType::Wired)
        {
            state = WIRED_CONNECTED_BUT_NOT_ACCESS_INTERNET;
            break;
        }
    }
    SET_TRAY_ICON_AND_CHECK_CONNECTIVITY_AND_RETURN(state);

    for (auto connection : list)
    {
        if (connection->type() == ConnectionSettings::ConnectionType::Wireless)
        {
            state = WIRELESS_CONNECTED_BUT_NOT_ACCESS_INTERNET;
            break;
        }
    }

    //最后如果既不是有线也不是无线，则显示有线网络图标
    if (state == UNKNOWN)
    {
        state = WIRED_CONNECTED_BUT_NOT_ACCESS_INTERNET;
    }
    SET_TRAY_ICON_AND_CHECK_CONNECTIVITY_AND_RETURN(state);
}

void NetworkTray::setTrayIcon(NetworkState state)
{
    auto stateInfo = m_StateInfoMap.value(state);
    setTrayIcon(stateInfo.iconPath, stateInfo.description);
}

void NetworkTray::setTrayIcon(const QString &iconPath, const QString &toolTip)
{
    QIcon icon;
    icon.addPixmap(NetworkUtils::trayIconColorSwitch(iconPath));
    icon.addPixmap(NetworkUtils::trayIconColorSwitch(iconPath, 64));
    m_systemTray->setIcon(icon);
    m_systemTray->setToolTip(toolTip);
}

// 重新获取device、初始化，刷新
// XXX:可以优化
void NetworkTray::handleDeviceAdded(const QString &devicePath)
{
    KLOG_DEBUG(qLcNetwork) << "Device Added:" << devicePath;
    Device::Ptr device = findNetworkInterface(devicePath);
    auto state = device->state();
    auto type = device->type();

    if (type == Device::Ethernet ||
        type == Device::Wifi)
    {
        if (state != Device::Unmanaged &&
            state != Device::Unavailable &&
            state != Device::UnknownState)
        {
            if (type == Device::Ethernet)
            {
                reloadWiredTrayPage();
            }
            else if (type == Device::Wifi)
            {
                reloadWirelessTrayPage();
            }
        }
        connect(device.data(), &Device::stateChanged, this, &NetworkTray::handleDeviceStateChanged, Qt::UniqueConnection);
    }
}

// XXX:当device被移除时，由于设备对象可能已经被删除，所以并不能通过findNetworkInterface(path)找到该设备接口，进而知道被删除的设备类型
void NetworkTray::handleDeviceRemoved(const QString &devicePath)
{
    KLOG_DEBUG(qLcNetwork) << "Device Removed :" << devicePath;
    if (m_wiredTrayPage != nullptr)
    {
        if (m_wiredTrayPage->devicePathList().contains(devicePath))
            reloadWiredTrayPage();
    }

    if (m_wirelessTrayPage != nullptr)
    {
        if (m_wirelessTrayPage->devicePathList().contains(devicePath))
            reloadWirelessTrayPage();
    }
}

// 处理Unmanaged和Unavailable的情况
// 一般来说使用deviceAdded/Removed信号即可，但是当出现Unavailable状态变化时，并没有发出deviceAdded/Removed信号
// Unmanaged状态的设备加入/删除时，会发出deviceAdded/Removed信号
void NetworkTray::handleDeviceStateChanged(NetworkManager::Device::State newstate,
                                           NetworkManager::Device::State oldstate,
                                           NetworkManager::Device::StateChangeReason reason)
{
    Device *device = qobject_cast<Device *>(sender());
    auto deviceType = device->type();
    KLOG_DEBUG(qLcNetwork) << "Device interfaceName:" << device->interfaceName();
    KLOG_DEBUG(qLcNetwork) << "Device newstate:" << newstate;
    KLOG_DEBUG(qLcNetwork) << "Device oldstate:" << oldstate;
    KLOG_DEBUG(qLcNetwork) << "Device reason:" << reason;

    // 设备变为可用
    if ((oldstate == Device::Unavailable || oldstate == Device::Unmanaged || oldstate == Device::UnknownState) &&
        (newstate != Device::Unmanaged && newstate != Device::Unavailable && newstate != Device::UnknownState))
    {
        if (deviceType == Device::Ethernet)
        {
            reloadWiredTrayPage();
            StatusNotification::generalNotify(tr("Wired network card: %1 available").arg(device->interfaceName()));
        }
        else if (deviceType == Device::Wifi)
        {
            reloadWirelessTrayPage();
            StatusNotification::generalNotify(tr("Wireless network card: %1 available").arg(device->interfaceName()));
        }

        if ((m_wiredTrayPage != nullptr) || (m_wirelessTrayPage != nullptr))
        {
            if (m_unavailableWidget != nullptr)
            {
                m_verticalLayout->removeWidget(m_unavailableWidget);
                m_unavailableWidget->deleteLater();
                m_unavailableWidget = nullptr;
                KLOG_DEBUG(qLcNetwork) << "remove unavailable widget";
            }
        }
    }

    QSet<Device::State> unavailableStates = {
        Device::Unavailable,
        Device::Unmanaged,
        Device::UnknownState};

    // 非休眠的情况下，从可用状态到不可用状态通知
    if (!unavailableStates.contains(oldstate) &&
        unavailableStates.contains(newstate) &&
        reason != Device::SleepingReason)
    {
        // 设备变为不可用时，如果无线和有线均不可用则显示网络不可用的提示
        KLOG_DEBUG(qLcNetwork) << "device is unavailable";
        if ((NetworkUtils::getAvailableDeviceList(Device::Ethernet).count() == 0) &&
            (NetworkUtils::getAvailableDeviceList(Device::Wifi).count() == 0))
        {
            UnavailableTrayPage();
            StatusNotification::generalNotify(tr("Network unavailable"));
            return;
        }

        if (deviceType == Device::Ethernet)
        {
            StatusNotification::generalNotify(tr("Wired network card: %1 unavailable").arg(device->interfaceName()));
            reloadWiredTrayPage();
        }
        else if (deviceType == Device::Wifi)
        {
            StatusNotification::generalNotify(tr("Wireless network card: %1 unavailable").arg(device->interfaceName()));
            reloadWirelessTrayPage();
        }
    }
}

void NetworkTray::handleDeviceManagedChanged()
{
}

// TODO:托盘对不可用状态进行提示
/**
 * XXX:由于在禁用和开启wifi时，并没有发出Wireless设备的deviceRemoved和deviceAdded信号
 * 并且当WirelessEnabledChanged信号发送时，device state 还处在unavailbel 不可用状态，需要处理
 */
void NetworkTray::handleWirelessEnabledChanged(bool enable)
{
    KLOG_DEBUG(qLcNetwork) << " Wireless Enabled Changed:" << enable;

    if (m_wirelessTrayPage != nullptr)
    {
        m_verticalLayout->removeWidget(m_wirelessTrayPage);
        delete m_wirelessTrayPage;
        m_wirelessTrayPage = nullptr;
    }
    m_wirelessDeviceList.clear();

    if (enable)
    {
        // networkInterfaces() 是否及时更新，需验证
        const Device::List deviceList = networkInterfaces();
        for (Device::Ptr dev : deviceList)
        {
            KLOG_DEBUG(qLcNetwork) << "dev->interfaceName():" << dev->interfaceName();
            KLOG_DEBUG(qLcNetwork) << "dev->state():" << dev->state();
            KLOG_DEBUG(qLcNetwork) << "dev->isValid():" << dev->isValid();

            if ((dev->state() == Device::Unmanaged) || (dev->state() == Device::UnknownState))
                continue;

            if (dev->type() == Device::Wifi)
            {
                // Note:此时WirelessDevice状态还处在Unavailable，但还是将该设备加载
                if (dev->state() == Device::Unavailable)
                {
                }
                m_wirelessDeviceList << dev;
            }
        }

        if (m_wirelessDeviceList.count() != 0)
        {
            m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
            m_verticalLayout->insertWidget(-1, m_wirelessTrayPage);
            m_verticalLayout->setMargin(0);
            update();
        }
    }
}

void NetworkTray::handleNetworkManagerStatusChanged(NetworkManager::Status status)
{
    KLOG_DEBUG(qLcNetwork) << "network status changed: " << status;
    updateTrayIcon();
}

void NetworkTray::handlePrimaryConnectionChanged(const QString &uni)
{
    KLOG_DEBUG(qLcNetwork) << "primary connection changed: " << uni;
    updateTrayIcon();
}

void NetworkTray::UnavailableTrayPage()
{
    if (m_wiredTrayPage != nullptr)
    {
        m_verticalLayout->removeWidget(m_wiredTrayPage);
        delete m_wiredTrayPage;
        m_wiredTrayPage = nullptr;
    }

    if (m_wirelessTrayPage != nullptr)
    {
        m_verticalLayout->removeWidget(m_wirelessTrayPage);
        delete m_wirelessTrayPage;
        m_wirelessTrayPage = nullptr;
    }

    if (m_unavailableWidget != nullptr)
    {
        return;
    }
    initUnavailableWidget();
    m_verticalLayout->addWidget(m_unavailableWidget);
    KLOG_DEBUG(qLcNetwork) << "add unavailable widget";
}

void NetworkTray::reloadWiredTrayPage()
{
    KLOG_DEBUG(qLcNetwork) << "reloadWiredTrayPage";
    if (m_wiredTrayPage != nullptr)
    {
        m_verticalLayout->removeWidget(m_wiredTrayPage);
        delete m_wiredTrayPage;
        m_wiredTrayPage = nullptr;
    }

    m_wiredDeviceList.clear();
    m_wiredDeviceList = NetworkUtils::getAvailableDeviceList(Device::Ethernet);
    if (m_wiredDeviceList.count() != 0)
    {
        m_wiredTrayPage = new TrayPage(m_wiredDeviceList, this);
        m_verticalLayout->insertWidget(0, m_wiredTrayPage);
        m_verticalLayout->setMargin(0);
        connect(m_wiredTrayPage, &TrayPage::sizeChanged, this, &NetworkTray::handleAdjustedTraySize, Qt::UniqueConnection);
        update();
    }
}

void NetworkTray::reloadWirelessTrayPage()
{
    KLOG_DEBUG(qLcNetwork) << "reloadWirelessTrayPage";
    if (m_wirelessTrayPage != nullptr)
    {
        m_verticalLayout->removeWidget(m_wirelessTrayPage);
        delete m_wirelessTrayPage;
        m_wirelessTrayPage = nullptr;
    }

    m_wirelessDeviceList.clear();
    m_wirelessDeviceList = NetworkUtils::getAvailableDeviceList(Device::Wifi);
    if (m_wirelessDeviceList.count() != 0)
    {
        m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
        m_verticalLayout->insertWidget(-1, m_wirelessTrayPage);
        m_verticalLayout->setMargin(0);

        connect(m_wirelessTrayPage, &TrayPage::sizeChanged, this, [this](QSize sizeHint)
                {
                    m_wirelessTraySizeHint = sizeHint;
                    m_wirelessTimer.start(); });
        update();
    }
}

// TODO:修改，重构
#define OFFSET_MARGIN 18
void NetworkTray::handleAdjustedTraySize(QSize sizeHint)
{
    // this->sizeHint() 更新不及时，需要等一段时间
    QTimer::singleShot(100, this, [=]()
                       {
                            if(m_wiredTrayPage && m_wirelessTrayPage)
                                adjustSize();
                            else
                            {
                                // KLOG_DEBUG(qLcNetwork) << "resize before this->size():" << this->size();
                                QSize newSize(this->sizeHint().width(),sizeHint.height() + OFFSET_MARGIN);
                                // KLOG_DEBUG(qLcNetwork) << "newSize:" << newSize;
                                this->resize(newSize);
                                KLOG_DEBUG(qLcNetwork) << "handleAdjustedTraySize";
                                // this->setFixedSize(newSize);
                                // KLOG_DEBUG(qLcNetwork) << "resize after this->size():" << this->size();
                            }
                            setTrayPagePos(); });
}

void NetworkTray::handleThemeChanged(Kiran::PaletteType paletteType)
{
    updateTrayIcon();
}

void NetworkTray::initTcpSocket()
{
    m_tcpClient = new QTcpSocket(this);
    connect(m_tcpClient, &QTcpSocket::connected, this, &NetworkTray::internetConnected);
    connect(m_tcpClient, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &NetworkTray::internetError);
}

void NetworkTray::checkInternetConnectivity()
{
    // NetworkManager::connectivity() 不准确，使用checkConnectivity
    QDBusPendingReply<uint> reply = NetworkManager::checkConnectivity();
    reply.waitForFinished();
    uint result = reply.value();
    if (result != NetworkManager::Connectivity::Full)
    {
        return;
    }

    QSettings confSettings(SETTINGS_PATH, QSettings::NativeFormat);
    QVariant enable = confSettings.value(QString("Network/CheckInternetConnectivity"));
    KLOG_DEBUG(qLcNetwork) << "check Internet Connectivity : " << enable;
    if (!enable.toBool())
    {
        internetConnected();
        return;
    }
    QVariant address = confSettings.value(QString("Network/Address"));
    QVariant port = confSettings.value(QString("Network/Port"));
    if(m_tcpClient->state() != QAbstractSocket::UnconnectedState)
    {
        m_tcpClient->abort();
    }   
    m_tcpClient->connectToHost(address.toString(), port.toInt());
}

void NetworkTray::internetConnected()
{
    KLOG_DEBUG(qLcNetwork) << "Connectivity check pass";
    NetworkState state;
    ActiveConnection::Ptr primaryActiveConnection = primaryConnection();
    if (!primaryActiveConnection.isNull())
    {
        if (primaryActiveConnection->type() == ConnectionSettings::Wireless)
        {
            state = WIRELESS_CONNECTED;
        }
        else
        {
            state = WIRED_CONNECTED;
        }
    }
    else
    {
        state = WIRED_CONNECTED;
    }
    setTrayIcon(state);

    m_tcpClient->abort();
}

void NetworkTray::internetError(QAbstractSocket::SocketError socketError)
{
    KLOG_INFO(qLcNetwork) << "Connectivity check fail: " << socketError;
    NetworkState state;
    if (primaryConnectionType() == ConnectionSettings::Wireless)
    {
        state = WIRELESS_CONNECTED_BUT_NOT_ACCESS_INTERNET;
    }
    else
    {
        state = WIRED_CONNECTED_BUT_NOT_ACCESS_INTERNET;
    }
    setTrayIcon(state);

    m_tcpClient->abort();
}
