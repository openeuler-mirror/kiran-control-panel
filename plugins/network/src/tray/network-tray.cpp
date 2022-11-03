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
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include "status-notifier-manager.h"
#include "tray-page.h"
#include "utils.h"
#include "wired-tray-widget.h"
#include "wireless-tray-widget.h"
#include "utils.h"
#include <QLabel>
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
    initUI();
    initMenu();
    initConnect();

    // XXX:现将widget移到屏幕外，防止第一次显示页面，由于没指定位置而闪现在左上角，之后统一修改页面显示逻辑
    this->move(-1000, -1000);

    //跟踪所有设备的状态变化
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        auto type = dev->type();
        if( type == Device::Ethernet || type == Device::Wifi)
        {
            connect(dev.data(),&Device::stateChanged,this,&NetworkTray::handleDeviceStateChanged,Qt::UniqueConnection);
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
                    KLOG_DEBUG() << "this device interface is not found";
                    return;
                }
                KLOG_DEBUG() << "device->uni():" << device->uni();
                KLOG_DEBUG() << "device->interfaceName():" << device->interfaceName();
                KLOG_DEBUG() << "device->state():" << device->state();
                KLOG_DEBUG() << "device->isValid():" << device->isValid();
                if(device->isValid())
                {
                    handleDeviceAdded(m_addDevicePath);
                }               
                else
                {
                    KLOG_INFO() << "this device interface is invalid!";
                    m_Timer.start();
                    KLOG_INFO() << "wait counts:" << m_waitCounts;
                } });

    m_Timer.setInterval(500);
    m_Timer.setSingleShot(true);
    m_waitCounts = 1;
    connect(&m_Timer, &QTimer::timeout, this, [this]()
            {
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                if(device.isNull())
                {
                    KLOG_DEBUG() << "this device interface is not found";
                    return ;
                }
                if(device->isValid())
                {
                    handleDeviceAdded(m_addDevicePath);
                    m_Timer.stop();
                }
                else
                {
                    KLOG_INFO() << "this device interface is not ready";
                    m_Timer.start();
                }
                m_waitCounts++;
                if(m_waitCounts > MAX_WAIT_COUNTS)
                {
                    KLOG_INFO() << "This device is not currently managed by NetworkManager";
                    m_Timer.stop();
                } });

    connect(notifier(), &Notifier::deviceRemoved, this, &NetworkTray::handleDeviceRemoved);

    connect(notifier(), &Notifier::statusChanged, this, &NetworkTray::handleNetworkManagerStatusChanged);

    connect(notifier(), &Notifier::primaryConnectionChanged, this, &NetworkTray::handlePrimaryConnectionChanged);

    connect(notifier(), &Notifier::wirelessEnabledChanged, this, &NetworkTray::handleWirelessEnabledChanged, Qt::UniqueConnection);

    // 无线网络如果一下消失多个网络，短时间会触发多次SizeHint变更的信号
    m_wirelessTimer.setInterval(10);
    m_wirelessTimer.setSingleShot(true);
    connect(m_wirelessTrayPage, &TrayPage::sizeChanged, this, [this](QSize sizeHint)
            {
                m_wirelessTraySizeHint = sizeHint;
                m_wirelessTimer.start(); });
    connect(&m_wirelessTimer, &QTimer::timeout, this, [this]()
            { handleAdjustedTraySize(m_wirelessTraySizeHint); });

    connect(m_wiredTrayPage, &TrayPage::sizeChanged, this, &NetworkTray::handleAdjustedTraySize, Qt::UniqueConnection);

    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &NetworkTray::handleThemeChanged);
}

void NetworkTray::initTrayIcon()
{
    m_systemTray = new QSystemTrayIcon();
    setTrayIcon(NetworkManager::status());
    KLOG_DEBUG() << " NetworkManager::status():" << NetworkManager::status();
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
    m_wiredDeviceList = NetworkUtils::getAvailableDeviceList(Device::Ethernet);
    m_wirelessDeviceList = NetworkUtils::getAvailableDeviceList(Device::Wifi);

    int wiredCount = m_wiredDeviceList.count();
    int wirelessCount = m_wirelessDeviceList.count();

    QWidget *widget = new QWidget(this);
    widget->setContentsMargins(0, 0, 0, 0);
    m_verticalLayout = new QVBoxLayout(widget);
    if(wiredCount == 0 && wirelessCount == 0)
    {
        if(m_unavailableWidget == nullptr)
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
        }
        if (wirelessCount != 0)
        {
            m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
            m_verticalLayout->addWidget(m_wirelessTrayPage);
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
        showTrayPage();
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
            KLOG_DEBUG() << "wireless Device name:" << wirelessDevice->interfaceName() << " requestScan error:" << replyRequestScan.error();
        }
        else
        {
            KLOG_DEBUG() << "wireless Device name:" << wirelessDevice->interfaceName() << " requestScan reply:" << replyRequestScan.reply();
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

void NetworkTray::showTrayPage()
{
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

void NetworkTray::setTrayPagePos()
{
    // KLOG_DEBUG() << "this->sizeHint():" << this->sizeHint();
    // KLOG_DEBUG() << "this->size():" << this->size();

    int pageHeight = this->size().height();
    int pageWidth = this->size().width();

    getTrayGeometry();
    // 抵消KiranRoundedTrayPopup的margin
    int offset = 8;
    this->move(m_xTray - pageWidth / 2, m_yTray - pageHeight + offset);
}

void NetworkTray::getTrayGeometry()
{
    // 名称待修改
    QDBusPendingReply<QString> getGeometry = m_statusNotifierManager->GetGeometry("~04-network");
    KLOG_DEBUG() << "getGeometry.value():" << getGeometry.value();

    double height, width, x, y;
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getGeometry.value().toLatin1(), &jsonParseError);
    if (!doc.isNull() && jsonParseError.error == QJsonParseError::NoError)
    {
        if (doc.isObject() && jsonParseError.error == QJsonParseError::NoError)
        {
            if (doc.isObject())
            {
                QJsonObject object = doc.object();
                QStringList list = object.keys();
                height = object.value("height").toDouble();
                width = object.value("width").toDouble();
                x = object.value("x").toDouble();
                y = object.value("y").toDouble();
            }
        }
    }
    m_heightTray = static_cast<int>(height);
    m_widthTray = static_cast<int>(width);
    m_xTray = static_cast<int>(x);
    m_yTray = static_cast<int>(y);
}

// TODO:增加其他状态图标
void NetworkTray::setTrayIcon(NetworkManager::Status status)
{
    QIcon icon;
    if (status == NetworkManager::Status::Connected)
    {
        // 判断主连接类型，托盘网络图标以主连接类型为准
        // NetworkManager::primaryConnectionType() 更新不及时，暂时不用
        ActiveConnection::Ptr primaryActiveConnection = primaryConnection();
        if (primaryActiveConnection != nullptr)
        {
            auto primaryConnectionType = primaryActiveConnection->connection()->settings()->connectionType();
            if (primaryConnectionType == ConnectionSettings::Wireless)
            {
                icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wireless-4.svg"));
                icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wireless-4.svg", 64));
            }
            else
            {
                icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-connection.svg"));
                icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-connection.svg", 64));
            }
        }
    }
    else if ((status == NetworkManager::Status::Disconnecting) || (status == NetworkManager::Status::Connecting))
    {
        // TODO:加载动画
        icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-disconnected.svg"));
        icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-disconnected.svg", 64));
    }
    else
    {
        icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-disconnected.svg"));
        icon.addPixmap(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired-disconnected.svg", 64));
    }
    m_systemTray->setIcon(icon);
}

// 重新获取device、初始化，刷新
// XXX:可以优化
void NetworkTray::handleDeviceAdded(const QString &devicePath)
{
    KLOG_DEBUG() << "Device Added:" << devicePath;
    Device::Ptr device = findNetworkInterface(devicePath);
    auto state = device->state();
    auto type = device->type();

    if(type == Device::Ethernet ||
        type == Device::Wifi)
    {
        if(state != Device::Unmanaged &&
            state != Device::Unavailable &&
            state != Device::UnknownState )
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
        connect(device.data(),&Device::stateChanged,this,&NetworkTray::handleDeviceStateChanged,Qt::UniqueConnection);
    }
}

// XXX:当device被移除时，由于设备对象可能已经被删除，所以并不能通过findNetworkInterface(path)找到该设备接口，进而知道被删除的设备类型
void NetworkTray::handleDeviceRemoved(const QString &devicePath)
{
    KLOG_DEBUG() << "Device Removed :" << devicePath;
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
//一般来说使用deviceAdded/Removed信号即可，但是当出现Unavailable状态变化时，并没有发出deviceAdded/Removed信号
//Unmanaged状态的设备加入/删除时，会发出deviceAdded/Removed信号
void NetworkTray::handleDeviceStateChanged(NetworkManager::Device::State newstate,
                                           NetworkManager::Device::State oldstate,
                                           NetworkManager::Device::StateChangeReason reason)
{

    // KLOG_DEBUG() << "newstate:" << newstate;
    // KLOG_DEBUG() << "oldstate:" << oldstate;
    // KLOG_DEBUG() << "reason:" << reason;
    Device *device = qobject_cast<Device *>(sender());
    auto deviceType = device->type();

    //设备变为可用
    if ((oldstate == Device::Unavailable || oldstate == Device::Unmanaged)
        &&
        (newstate != Device::Unmanaged && newstate != Device::Unavailable && newstate != Device::UnknownState))
    {
        if(m_unavailableWidget != nullptr)
        {
            m_verticalLayout->removeWidget(m_unavailableWidget);
            m_unavailableWidget->deleteLater();
            m_unavailableWidget = nullptr;
        }

        if (deviceType == Device::Ethernet)
        {
            reloadWiredTrayPage();
        }
        else if (deviceType == Device::Wifi)
        {
            reloadWirelessTrayPage();
        }
    }

    //设备变为不可用时，如果无线和有线均不可用则显示网络不可用的提示
    if(newstate == Device::Unavailable || newstate == Device::Unmanaged
        || newstate == Device::UnknownState)
    {
        if (deviceType == Device::Ethernet)
        {
            reloadWiredTrayPage();
        }
        else if (deviceType == Device::Wifi)
        {
            reloadWirelessTrayPage();
        }

        if(m_wiredTrayPage == nullptr && m_wirelessTrayPage == nullptr)
        {
            initUnavailableWidget();
            m_verticalLayout->addWidget(m_unavailableWidget);
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
    KLOG_DEBUG() << " Wireless Enabled Changed:" << enable;

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
            KLOG_DEBUG() << "dev->interfaceName():" << dev->interfaceName();
            KLOG_DEBUG() << "dev->state():" << dev->state();
            KLOG_DEBUG() << "dev->isValid():" << dev->isValid();

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
    setTrayIcon(status);
}

void NetworkTray::handlePrimaryConnectionChanged(const QString &uni)
{
    setTrayIcon(NetworkManager::status());
}

void NetworkTray::reloadWiredTrayPage()
{
    KLOG_DEBUG() << "reloadWiredTrayPage";
    m_verticalLayout->removeWidget(m_wiredTrayPage);
    m_wiredTrayPage->disconnect();
    delete m_wiredTrayPage;
    m_wiredTrayPage = nullptr;
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
    m_verticalLayout->removeWidget(m_wirelessTrayPage);
    m_wiredTrayPage->disconnect();
    delete m_wirelessTrayPage;
    m_wirelessTrayPage = nullptr;
    m_wirelessDeviceList.clear();

    m_wirelessDeviceList = NetworkUtils::getAvailableDeviceList(Device::Wifi);
    if (m_wirelessDeviceList.count() != 0)
    {
        m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
        m_verticalLayout->insertWidget(-1, m_wirelessTrayPage);
        m_verticalLayout->setMargin(0);

        // 无线网络如果一下消失多个网络，短时间会触发多次SizeHint变更的信号
        m_wirelessTimer.setInterval(10);
        m_wirelessTimer.setSingleShot(true);
        connect(m_wirelessTrayPage, &TrayPage::sizeChanged, this, [this](QSize sizeHint)
                {
                    m_wirelessTraySizeHint = sizeHint;
                    m_wirelessTimer.start(); });
        connect(&m_wirelessTimer, &QTimer::timeout, this, [this]()
                { handleAdjustedTraySize(m_wirelessTraySizeHint); });
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
                                // KLOG_DEBUG() << "resize before this->size():" << this->size();
                                QSize newSize(this->sizeHint().width(),sizeHint.height() + OFFSET_MARGIN);
                                // KLOG_DEBUG() << "newSize:" << newSize;
                                this->resize(newSize);
                                KLOG_DEBUG() << "handleAdjustedTraySize";
                                // this->setFixedSize(newSize);
                                // KLOG_DEBUG() << "resize after this->size():" << this->size();
                            }
                            setTrayPagePos(); });
}

void NetworkTray::handleThemeChanged(Kiran::PaletteType paletteType)
{
    setTrayIcon(NetworkManager::status());
}
