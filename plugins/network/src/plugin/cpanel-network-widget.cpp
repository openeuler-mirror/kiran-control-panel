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

#include "cpanel-network-widget.h"
#include <kiran-sidebar-item.h>
#include <qt5-log-i.h>
#include "details-page/details-page.h"
#include "logging-category.h"
#include "prefs.h"
#include "signal-forward.h"
#include "ui_cpanel-network-widget.h"
#include "utils.h"
#include "vpn-manager.h"
#include "wired-manager.h"
#include "wireless-manager.h"

using namespace NetworkManager;
using namespace Kiran::Theme;
#define MAX_WAIT_COUNTS 10

enum NetworkSidebarItems
{
    ITEM_WIRED,
    ITEM_WIRELESS,
    ITEM_VPN,
    ITEM_DETAILS,
};

#define NETWORK_SIDEBAR_ITEM "NetworkSidebarItem"
#define SIDERBAR_ITEM_TYPE_ROLE Qt::UserRole + 100

CPanelNetworkWidget::CPanelNetworkWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CPanelNetworkWidget)
{
    qRegisterMetaType<NetworkConnectionInfo>("NetworkConnectionInfo");
    ui->setupUi(this);
    init();
}

CPanelNetworkWidget::~CPanelNetworkWidget()
{
    delete ui;
}

void CPanelNetworkWidget::init()
{
    ui->sidebar->viewport()->setAutoFillBackground(false);
    initPage();
    initConnect();
}

void CPanelNetworkWidget::initPage()
{
    m_subItemsList.clear();

    initWiredManager();
    initWirelessManager();

    VpnManager *vpnManager = new VpnManager(this);
    vpnManager->setProperty(NETWORK_SIDEBAR_ITEM, ITEM_VPN);

    ui->stackedWidget->addWidget(vpnManager);

    KiranSidebarItem *sidebarItem = new KiranSidebarItem(tr("VPN"));
    ui->sidebar->addItem(sidebarItem);
    sidebarItem->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/vpn.svg"));
    m_subItemsList << tr("VPN");

    DetailsPage *networkDetails = new DetailsPage(this);
    networkDetails->setProperty(NETWORK_SIDEBAR_ITEM, ITEM_DETAILS);

    ui->stackedWidget->addWidget(networkDetails);
    ui->sidebar->addItem(tr("Network Details"));
    auto lastItem = ui->sidebar->item(ui->sidebar->count() - 1);
    lastItem->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/network-details.svg"));
    m_subItemsList << tr("Network Details");

    setCurrentSubItem(0);
}

// Note: 处理设备插拔情况，使用deviceAdded/deviceRemoved，但之前偶现过插拔时没有deviceAdded/deviceRemoved信号发出
//  增加Device::managedChanged 信号处理设备插拔，增加冗余度
void CPanelNetworkWidget::initConnect()
{
    // Note:新设备插入后，需要等待一段时间，Device::List networkInterfaces() 来不及更新
    // Note:DeviceAdded signal is emitted when a new network interface is available.

    // XXX:当发出deviceAdded信号时，应该已经managed，需要修改并重新测试
    // deviceAdded信号发出时，根据信号的定义，此时device state为managed，但实际上并为unmanaged

    connect(SignalForward::instance(), &SignalForward::wirelessDeviceAdded, this, &CPanelNetworkWidget::addWirelessDevice);
    connect(SignalForward::instance(), &SignalForward::wiredDeviceAdded, this, &CPanelNetworkWidget::addWiredDevice);

    connect(notifier(), &Notifier::deviceRemoved, this, &CPanelNetworkWidget::removeDevice);

    connect(notifier(), &Notifier::wirelessEnabledChanged, this, &CPanelNetworkWidget::handleWirelessEnabledChanged);
    connect(notifier(), &Notifier::networkingEnabledChanged, this, &CPanelNetworkWidget::handleNetworkingEnabledChanged);

    connect(ui->sidebar, &QListWidget::currentItemChanged, this, &CPanelNetworkWidget::changeSideBarItem);

    connect(DEFAULT_PALETTE(), &Palette::baseColorsChanged, this, &CPanelNetworkWidget::changeTheme);
}

void CPanelNetworkWidget::initWiredManager()
{
    auto wiredDeviceList = NetworkUtils::getManagedDeviceList(Device::Ethernet);
    if (wiredDeviceList.count() == 0)
    {
        return;
    }

    WiredManager *wiredManager = new WiredManager("", this);
    wiredManager->setProperty(NETWORK_SIDEBAR_ITEM, ITEM_WIRED);
    ui->stackedWidget->insertWidget(0, wiredManager);

    KiranSidebarItem *wiredSidebarItem = new KiranSidebarItem();
    wiredSidebarItem->setText(tr("Wired Network"));
    wiredSidebarItem->setData(SIDERBAR_ITEM_TYPE_ROLE, Device::Ethernet);
    m_subItemsList << tr("Wired Network");
    ui->sidebar->insertItem(0, wiredSidebarItem);
    m_kiranSidebarItems << wiredSidebarItem;

    wiredSidebarItem->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired.svg"));
    setSidebarItemStatus(wiredSidebarItem, Device::State::Disconnected);

    for (int i = 0; i < wiredDeviceList.count(); i++)
    {
        Device::Ptr device = wiredDeviceList.value(i);
        connect(device.data(), &Device::stateChanged, this, &CPanelNetworkWidget::changeDeviceState, Qt::UniqueConnection);

        if (Network::Prefs::instance()->getCheckWiredCarrier())
        {
            auto wiredDevice = qobject_cast<WiredDevice *>(device.data());
            connect(wiredDevice, &WiredDevice::carrierChanged, this, [this]()
                    { updateSidebarItemStatus(NetworkManager::Device::Ethernet); });
        }
    }
    updateSidebarItemStatus(NetworkManager::Device::Ethernet);
}

void CPanelNetworkWidget::initWirelessManager()
{
    auto wirelessDeviceList = NetworkUtils::getManagedDeviceList(Device::Wifi);
    if (wirelessDeviceList.count() == 0)
    {
        return;
    }

    WirelessManager *wirelessManager = new WirelessManager("", this);
    wirelessManager->setProperty(NETWORK_SIDEBAR_ITEM, ITEM_WIRELESS);
    ui->stackedWidget->insertWidget(0, wirelessManager);

    KiranSidebarItem *wirelessSidebarItem = new KiranSidebarItem();
    wirelessSidebarItem->setText(tr("Wireless Network"));
    wirelessSidebarItem->setData(SIDERBAR_ITEM_TYPE_ROLE, Device::Wifi);
    m_subItemsList << tr("Wireless Network");

    ui->sidebar->insertItem(0, wirelessSidebarItem);
    m_kiranSidebarItems << wirelessSidebarItem;

    wirelessSidebarItem->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wireless.svg"));
    setSidebarItemStatus(wirelessSidebarItem, Device::State::Disconnected);

    for (int i = 0; i < wirelessDeviceList.count(); i++)
    {
        Device::Ptr device = wirelessDeviceList.value(i);
        connect(device.data(), &Device::stateChanged, this, &CPanelNetworkWidget::changeDeviceState, Qt::UniqueConnection);
    }
    updateSidebarItemStatus(NetworkManager::Device::Wifi);
}

void CPanelNetworkWidget::changeDeviceState(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    KLOG_DEBUG(qLcNetwork) << "new state:" << newstate;
    KLOG_DEBUG(qLcNetwork) << "old state:" << oldstate;
    KLOG_DEBUG(qLcNetwork) << "reason:" << reason;
    auto device = qobject_cast<NetworkManager::Device *>(sender());
    if (device == nullptr)
    {
        KLOG_DEBUG(qLcNetwork) << "device is null";
        return;
    }

    updateSidebarItemStatus(device->type());
}

void CPanelNetworkWidget::addWirelessDevice(const QString &devicePath)
{
    auto wirelessDevice = findNetworkInterface(devicePath);
    connect(wirelessDevice.data(), &Device::stateChanged, this, &CPanelNetworkWidget::changeDeviceState, Qt::UniqueConnection);
    if (!isExistWirelessItem())
    {
        initWirelessManager();
        emit subItemsChanged();
    }
}

void CPanelNetworkWidget::addWiredDevice(const QString &devicePath)
{
    auto wiredDevice = findNetworkInterface(devicePath);
    connect(wiredDevice.data(), &Device::stateChanged, this, &CPanelNetworkWidget::changeDeviceState, Qt::UniqueConnection);
    if (!isExistWiredItem())
    {
        initWiredManager();
        emit subItemsChanged();
    }
}

void CPanelNetworkWidget::updateSidebarItemStatus(NetworkManager::Device::Type deviceType)
{
    QString sidebarItemText;
    Device::State state = Device::Disconnected;

    if (deviceType != Device::Ethernet &&
        deviceType != Device::Wifi)
    {
        return;
    }

    auto deviceList = NetworkUtils::getManagedDeviceList(deviceType);
    for (auto device : deviceList)
    {
        bool checkWiredCarrier = Network::Prefs::instance()->getCheckWiredCarrier();
        auto wireDevice = qobject_cast<WiredDevice *>(device);
        if (checkWiredCarrier && wireDevice)
        {
            if (wireDevice->carrier() && device->state() == Device::State::Activated)
            {
                state = Device::State::Activated;
                break;
            }
        }
        else if (device->state() == Device::State::Activated)
        {
            state = Device::State::Activated;
            break;
        }
    }

    KiranSidebarItem *sidebarItem = nullptr;
    for (auto item : m_kiranSidebarItems)
    {
        if (item->data(SIDERBAR_ITEM_TYPE_ROLE).toInt() == deviceType)
        {
            sidebarItem = item;
            break;
        }
    }
    setSidebarItemStatus(sidebarItem, state);
}

void CPanelNetworkWidget::setSidebarItemStatus(KiranSidebarItem *sidebarItem, NetworkManager::Device::State state)
{
    if (sidebarItem == nullptr)
    {
        return;
    }

    if (state == Device::State::Activated)
    {
        sidebarItem->setStatusDesc(tr("Connected"), "");
    }
    else if (state == Device::State::Unavailable)  // 对应拔出网线 -- 对应禁用
    {
        sidebarItem->setStatusDesc(tr("Unavailable"), "");
    }
    else
    {
        sidebarItem->setStatusDesc(tr("Disconnected"), "");
    }
}

QStringList CPanelNetworkWidget::subItemNameList()
{
    return m_subItemsList;
}

void CPanelNetworkWidget::setCurrentSubItem(int index)
{
    ui->sidebar->setCurrentRow(index);
    changeSideBarItem(ui->sidebar->currentItem());
}

void CPanelNetworkWidget::changeTheme()
{
    for (int i = 0; i < ui->sidebar->count(); ++i)
    {
        auto icon = ui->sidebar->item(i)->icon();
        QPixmap pixmap = icon.pixmap(16, 16);
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
        ui->sidebar->item(i)->setIcon(pixmap);
    }
}

//  ui->stackedWidget与ui->sidebar的index对应
void CPanelNetworkWidget::changeSideBarItem(QListWidgetItem *item)
{
    ui->stackedWidget->setCurrentIndex(ui->sidebar->row(item));

    QString itemText = item->text();
    if (itemText.contains(tr("Wireless Network")))
    {
        auto wirelessDeviceList = NetworkUtils::getManagedDeviceList(Device::Wifi);
        foreach (auto device, wirelessDeviceList)
        {
            WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice *>(device);
            QDBusPendingReply<> replyRequestScan = wirelessDevice->requestScan();

            replyRequestScan.waitForFinished();
            if (replyRequestScan.isError())
            {
                KLOG_DEBUG(qLcNetwork) << "wireless Device name:" << wirelessDevice->interfaceName() << " requestScan error:" << replyRequestScan.error();
            }
        }
    }
}

void CPanelNetworkWidget::removeDevice(const QString &devicePath)
{
    KLOG_DEBUG(qLcNetwork) << "DeviceRemoved: " << devicePath;

    auto newWiredDeviceList = NetworkUtils::getManagedDeviceList(Device::Ethernet);
    auto newWirelessDeviceList = NetworkUtils::getManagedDeviceList(Device::Wifi);

    if (newWiredDeviceList.count() == 0 && isExistWiredItem())
    {
        QString text = tr("Wired Network");
        removeSidebarItem(text, ITEM_WIRED);
        emit subItemsChanged();
    }

    if (newWirelessDeviceList.count() == 0 && isExistWirelessItem())
    {
        QString text = tr("Wireless Network");
        removeSidebarItem(text, ITEM_WIRELESS);
        emit subItemsChanged();
    }
}

void CPanelNetworkWidget::removeSidebarItem(const QString &text, int index)
{
    for (auto item : m_kiranSidebarItems)
    {
        if (item->text() == text)
        {
            m_kiranSidebarItems.removeOne(item);
            delete item;
            break;
        }
    }
    m_subItemsList.removeAll(text);

    for (int i = 0; i < ui->stackedWidget->count(); i++)
    {
        auto widget = ui->stackedWidget->widget(i);
        if (widget->property(NETWORK_SIDEBAR_ITEM).toInt() == index)
            ;
        {
            ui->stackedWidget->removeWidget(widget);
            delete widget;
            break;
        }
    }
}

bool CPanelNetworkWidget::isExistWiredItem()
{
    for (auto item : m_kiranSidebarItems)
    {
        if (item->data(SIDERBAR_ITEM_TYPE_ROLE).toInt() == Device::Ethernet)
        {
            return true;
        }
    }
    return false;
}

bool CPanelNetworkWidget::isExistWirelessItem()
{
    for (auto item : m_kiranSidebarItems)
    {
        if (item->data(SIDERBAR_ITEM_TYPE_ROLE).toInt() == Device::Wifi)
        {
            return true;
        }
    }
    return false;
}

void CPanelNetworkWidget::handleWirelessEnabledChanged(bool enable)
{
}

void CPanelNetworkWidget::handleNetworkingEnabledChanged(bool enable)
{
}
