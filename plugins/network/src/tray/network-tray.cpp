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

#include "network-tray.h"
#include <qt5-log-i.h>
#include <style-palette.h>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include "status-notifier-manager.h"
#include "tray-page.h"
#include "wired-tray-widget.h"
#include "wireless-tray-widget.h"

using namespace NetworkManager;

#define STATUS_NOTIFIER_MANAGER "org.kde.StatusNotifierManager"
#define STATUS_NOTIFIER_MANAGER_OBJECT_NAME "/StatusNotifierManager"
#define MAX_WAIT_COUNTS 10

NetworkTray::NetworkTray(QWidget *parent) : KiranRoundedTrayPopup(parent),
                                            m_wiredTrayPage(nullptr),
                                            m_wirelessTrayPage(nullptr)
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
}

void NetworkTray::initUI()
{
    initTrayPage();
    initTrayIcon();
    QWidget *widget = new QWidget(this);
    widget->setContentsMargins(0, 0, 0, 0);
    m_verticalLayout = new QVBoxLayout(widget);
    if (m_wiredTrayPage != nullptr)
        m_verticalLayout->addWidget(m_wiredTrayPage);
    if (m_wirelessTrayPage != nullptr)
        m_verticalLayout->addWidget(m_wirelessTrayPage);
    m_verticalLayout->setMargin(0);
    m_verticalLayout->setSpacing(0);
    m_verticalLayout->addStretch();
    setContentWidget(widget);
    setContentsMargins(0, 0, 0, 0);
    setMaximumHeight(868);
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
                if(device->managed())
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
    m_wirelessTimer.setInterval(100);
    m_wirelessTimer.setSingleShot(true);
    connect(m_wirelessTrayPage, &TrayPage::adjustedTraySize, [this](QSize sizeHint)
            {
                m_wirelessTraySizeHint = sizeHint;
                m_wirelessTimer.start(); });
    connect(&m_wirelessTimer, &QTimer::timeout, [this]()
            { handleAdjustedTraySize(m_wirelessTraySizeHint); });

    connect(m_wiredTrayPage, &TrayPage::adjustedTraySize, this, &NetworkTray::handleAdjustedTraySize);

    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &NetworkTray::handleThemeChanged);
}

void NetworkTray::initTrayIcon()
{
    m_systemTray = new QSystemTrayIcon();
    setTrayIcon(NetworkManager::status());
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

// 初始化条件：设备存在且可用
void NetworkTray::initTrayPage()
{
    getAvailableDeviceList();
    if (m_wiredDeviceList.count() != 0)
        m_wiredTrayPage = new TrayPage(m_wiredDeviceList, this);
    if (m_wirelessDeviceList.count() != 0)
        m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
}

void NetworkTray::getAvailableDeviceList()
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        KLOG_DEBUG() << "dev->interfaceName():" << dev->interfaceName();
        KLOG_DEBUG() << "dev->state():" << dev->state();
        KLOG_DEBUG() << "dev->isValid():" << dev->isValid();

        if (dev->state() == Device::Unavailable)
            continue;
        if (dev->state() == Device::Unmanaged)
            continue;

        switch (dev->type())
        {
        case Device::Ethernet:
            m_wiredDeviceList << dev;
            break;
        case Device::Wifi:
            m_wirelessDeviceList << dev;
            break;
        default:
            break;
        }
    }
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
    process.startDetached("kiran-control-panel -c network");
}

void NetworkTray::showTrayPage()
{
    // XXX:托盘界面在不可见的情况，不方便去修改size和位置，暂时先显示后在调整大小和位置
    this->show();
    QTimer::singleShot(50, this, [=]()
                       {
                           adjustSize();
                           setTrayPagePos(); });
}

void NetworkTray::setTrayPagePos()
{
    getTrayGeometry();
    int pageHeight = this->size().height();
    int pageWidth = this->size().width();
    KLOG_DEBUG() << "this->sizeHint():" << this->sizeHint();
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
                // ActiveConnection::Ptr primaryActiveConnection  = primaryConnection();
                // WirelessSetting::Ptr wirelessSetting = primaryActiveConnection->connection()->settings()->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
                // QString ssid = QString(wirelessSetting->ssid());

                // QString devicePath = primaryActiveConnection->devices().value(0);
                // Device::Ptr device = findNetworkInterface(devicePath);
                // WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice*>(device);
                // WirelessNetwork::Ptr wirelessNetwork = wirelessDevice->findNetwork(ssid);
                // int signalStrength = wirelessNetwork->signalStrength();

                m_systemTray->setIcon(trayIconColorSwitch(":/kcp-network-images/wireless-4.svg"));
            }
        }
        else
        {
            // 可用
            m_systemTray->setIcon(trayIconColorSwitch(":/kcp-network-images/wired-connection.svg"));
        }
    }
    else if (status == NetworkManager::Status::Disconnecting || NetworkManager::Status::Connecting)
    {
        // TODO:加载动画
    }
    else
    {
        m_systemTray->setIcon(trayIconColorSwitch(":/kcp-network-images/wired-disconnected.svg"));
    }
}

// 重新获取device、初始化，刷新
// XXX:可以优化
void NetworkTray::handleDeviceAdded(const QString &devicePath)
{
    KLOG_DEBUG() << "---------------------handleDeviceAdded:" << devicePath;
    Device::Ptr device = findNetworkInterface(devicePath);
    if (device->type() == Device::Ethernet)
    {
        reloadWiredTrayPage();
    }
    else if (device->type() == Device::Wifi)
    {
        reloadWirelessTrayPage();
    }
}

// XXX:当device被移除时，由于设备对象可能已经被删除，所以并不能通过findNetworkInterface(path)找到该设备接口，进而知道被删除的设备类型
void NetworkTray::handleDeviceRemoved(const QString &devicePath)
{
    KLOG_DEBUG() << "handle Device Removed :" << devicePath;
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

// TODO:处理Unmanaged和Unavailable的情况
void NetworkTray::handleDeviceStateChanged(NetworkManager::Device::State newstate,
                                           NetworkManager::Device::State oldstate,
                                           NetworkManager::Device::StateChangeReason reason)
{
    if ((oldstate == Device::Unavailable) &&
        (newstate != Device::Unmanaged && newstate != Device::UnknownState))
    {
    }

    if (newstate == Device::State::Unavailable)
    {
        Device *device = qobject_cast<Device *>(sender());
        // KLOG_DEBUG() << "Device::State::Unavailable";
        // KLOG_DEBUG() << "device->uni():" << device->uni();
        // KLOG_DEBUG() << "device->interfaceName():" << device->interfaceName();
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

// TODO:托盘对不可用状态进行提示
void NetworkTray::handleWirelessEnabledChanged(bool enable)
{
    KLOG_DEBUG() << "-----------------------handleWirelessEnabledChanged:" << enable;

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
    m_verticalLayout->removeWidget(m_wiredTrayPage);
    delete m_wiredTrayPage;
    m_wiredTrayPage = nullptr;
    m_wiredDeviceList.clear();

    getAvailableDeviceList();
    if (m_wiredDeviceList.count() != 0)
    {
        m_wiredTrayPage = new TrayPage(m_wiredDeviceList, this);
        m_verticalLayout->insertWidget(0, m_wiredTrayPage);
        m_verticalLayout->setMargin(0);
        update();
    }
}

void NetworkTray::reloadWirelessTrayPage()
{
    m_verticalLayout->removeWidget(m_wirelessTrayPage);
    delete m_wirelessTrayPage;
    m_wirelessTrayPage = nullptr;
    m_wirelessDeviceList.clear();

    getAvailableDeviceList();
    if (m_wirelessDeviceList.count() != 0)
    {
        m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
        m_verticalLayout->insertWidget(-1, m_wirelessTrayPage);
        m_verticalLayout->setMargin(0);
        update();
    }
}

void NetworkTray::handleAdjustedTraySize(QSize sizeHint)
{
    // this->sizeHint() 更新不及时，需要等一段时间
    QTimer::singleShot(100, this, [=]()
                       {
                            adjustSize();
                            setTrayPagePos(); });
}

void NetworkTray::handleThemeChanged(Kiran::PaletteType paletteType)
{
    setTrayIcon(NetworkManager::status());
}

QPixmap NetworkTray::trayIconColorSwitch(const QString &iconPath)
{
    // icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(16, 16);
    if (Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }

    return pixmap;
}

void NetworkTray::paintEvent(QPaintEvent *event)
{
    auto stylePalette = Kiran::StylePalette::instance();
    auto borderColor = stylePalette->color(Kiran::StylePalette::Normal, Kiran::StylePalette::Window, Kiran::StylePalette::Border);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath painterPath;
    // 抵消KiranRoundedTrayPopup的margin
    int offset = 8;

    QRectF frect(rect().adjusted(8, 8, -8, -8));
    painterPath.addRoundedRect(frect, 4, 4);

    painter.setPen(borderColor);
    painter.drawPath(painterPath);

    // auto pen = painter.pen();
    // pen.setWidth(1);
    // pen.setColor(borderColor);
    // painter.strokePath(painterPath, pen);

    QWidget::paintEvent(event);
}