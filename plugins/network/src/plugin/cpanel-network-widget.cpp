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
#include "ui_cpanel-network-widget.h"
#include "utils.h"
#include "vpn-manager.h"
#include "wired-manager.h"
#include "wireless-manager.h"

using namespace NetworkManager;
#define MAX_WAIT_COUNTS 10

enum NetworkSettingsPages
{
    PAGE_WIRED,
    PAGE_WIRELESS,
    PAGE_VPN
};

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

// ui->stackedWidget与ui->sidebar的index对应
void CPanelNetworkWidget::initPage()
{
    m_subItemsList.clear();
    m_wiredDeviceList = NetworkUtils::getManagedDeviceList(Device::Ethernet);
    m_wirelessDeviceList = NetworkUtils::getManagedDeviceList(Device::Wifi);

    int row = 0;
    for (int i = 0; i < m_wiredDeviceList.count(); i++)
    {
        Device::Ptr device = m_wiredDeviceList.value(i);
        QString devicePath = device->uni();
        QString deviceName = device->interfaceName();
        WiredManager *wiredManager = new WiredManager(devicePath, this);
        ui->stackedWidget->insertWidget(row, wiredManager);

        KiranSidebarItem *sidebarItem = new KiranSidebarItem();
        sidebarItem->setText(deviceName);
        m_subItemsList << deviceName;
        ui->sidebar->insertItem(row, sidebarItem);
        m_deviceToSidebarItem.insert(devicePath, sidebarItem);
        sidebarItem->setData(Qt::UserRole, row);
        sidebarItem->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wired.svg"));
        setSidebarItemStatus(sidebarItem, device->state());
        row++;

        connect(device.data(), &Device::managedChanged, this, &CPanelNetworkWidget::handleManagedChanged, Qt::UniqueConnection);
        connect(device.data(), &Device::stateChanged, this, &CPanelNetworkWidget::handleStateChanged, Qt::UniqueConnection);
    }

    for (int i = 0; i < m_wirelessDeviceList.count(); i++)
    {
        Device::Ptr device = m_wirelessDeviceList.value(i);
        QString devicePath = device->uni();
        QString deviceName = device->interfaceName();
        WirelessManager *wirelessManager = new WirelessManager(devicePath, this);
        ui->stackedWidget->insertWidget(row, wirelessManager);

        KiranSidebarItem *sidebarItem = new KiranSidebarItem();
        sidebarItem->setText(deviceName);
        m_subItemsList << deviceName;
        ui->sidebar->insertItem(row, sidebarItem);

        m_deviceToSidebarItem.insert(devicePath, sidebarItem);
        sidebarItem->setData(Qt::UserRole, row);
        sidebarItem->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/wireless.svg"));
        setSidebarItemStatus(sidebarItem, device->state());
        row++;

        connect(device.data(), &Device::managedChanged, this, &CPanelNetworkWidget::handleManagedChanged, Qt::UniqueConnection);
        connect(device.data(), &Device::stateChanged, this, &CPanelNetworkWidget::handleStateChanged, Qt::UniqueConnection);
    }

    // TODO:是否要添加VPN的sidebarItem状态描述
    VpnManager *vpnManager = new VpnManager(this);
    ui->stackedWidget->insertWidget(row, vpnManager);

    KiranSidebarItem *sidebarItem = new KiranSidebarItem(tr("VPN"));
    ui->sidebar->insertItem(row, sidebarItem);
    ui->sidebar->item(row)->setData(Qt::UserRole, row);
    ui->sidebar->item(row)->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/vpn.svg"));
    m_subItemsList << tr("VPN");
    row++;

    DetailsPage *networkDetails = new DetailsPage(this);
    ui->stackedWidget->insertWidget(row, networkDetails);
    ui->sidebar->insertItem(row, tr("Network Details"));
    ui->sidebar->item(row)->setData(Qt::UserRole, row);
    ui->sidebar->item(row)->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/network-details.svg"));
    m_subItemsList << tr("Network Details");
    row++;

    ui->sidebar->setCurrentRow(0);
}

//Note: 处理设备插拔情况，使用deviceAdded/deviceRemoved，但之前偶现过插拔时没有deviceAdded/deviceRemoved信号发出
// 增加Device::managedChanged 信号处理设备插拔，增加冗余度
void CPanelNetworkWidget::initConnect()
{
    m_Timer.setInterval(500);
    m_Timer.setSingleShot(true);
    m_waitCounts = 1;
    // Note:新设备插入后，需要等待一段时间，Device::List networkInterfaces() 来不及更新
    // Note:DeviceAdded signal is emitted when a new network interface is available.

    // XXX:当发出deviceAdded信号时，应该已经managed，需要修改并重新测试
    // deviceAdded信号发出时，根据信号的定义，此时device state为managed，但实际上并为unmanaged
    connect(notifier(), &Notifier::deviceAdded, this, [this](const QString &uni)
            {
                m_addDevicePath = uni;
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                if(device.isNull())
                {
                    KLOG_DEBUG() << "this device interface is not found";
                    return;
                }
                if(device->managed())
                {
                    handleDeviceAdded(m_addDevicePath);
                }
                else
                {
                    KLOG_INFO() << "this device interface is invalid!";
                    m_Timer.start();
                    KLOG_INFO() << "wait counts:" << m_waitCounts;
                } });

    connect(&m_Timer, &QTimer::timeout, this, [this]()
            {
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                if(device->managed())
                {
                    handleDeviceAdded(m_addDevicePath);
                    m_Timer.stop();
                }
                else
                {
                    KLOG_INFO() << "this device interface is invalid!";
                    m_Timer.start();
                }
                m_waitCounts++;
                if(m_waitCounts > MAX_WAIT_COUNTS)
                {
                    KLOG_INFO() << "This device is currently invalid by NetworkManager";
                    m_Timer.stop();
                } });

    connect(notifier(), &Notifier::deviceRemoved, this, &CPanelNetworkWidget::handleDeviceRemoved);

    connect(notifier(), &Notifier::wirelessEnabledChanged, this, &CPanelNetworkWidget::handleWirelessEnabledChanged);
    connect(notifier(), &Notifier::networkingEnabledChanged, this, &CPanelNetworkWidget::handleNetworkingEnabledChanged);

    connect(ui->sidebar, &QListWidget::itemClicked, this, &CPanelNetworkWidget::handleSideBarItemClicked);

    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &CPanelNetworkWidget::handleThemeChanged);
}


void CPanelNetworkWidget::handleManagedChanged()
{
    auto device = qobject_cast<Device *>(sender());
    if (device != nullptr)
    {
        if (device->managed())
        {
        }
    }
    KLOG_DEBUG() << "Device Managed Changed";
    reload();
    emit subItemsChanged();
}

void CPanelNetworkWidget::handleStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    KLOG_DEBUG() << "---------newstate:" << newstate;
    KLOG_DEBUG() << "---------oldstate:" << oldstate;
    KLOG_DEBUG() << "---------reason:" << reason;
    auto device = qobject_cast<NetworkManager::Device *>(sender());
    if (device == nullptr)
    {
        KLOG_DEBUG() << "device ptr is null";
        return;
    }

    QString devicePath = device->uni();
    KiranSidebarItem *item = m_deviceToSidebarItem.value(devicePath);
    setSidebarItemStatus(item, newstate);
}

void CPanelNetworkWidget::setSidebarItemStatus(KiranSidebarItem *sidebarItem, NetworkManager::Device::State state)
{
    if (sidebarItem != nullptr)
    {
        if (state == Device::State::Activated)
        {
            sidebarItem->setStatusDesc(tr("Connected"), "");
        }
        else if (state == Device::State::Unavailable) //对应拔出网线 -- 对应禁用
        {
            sidebarItem->setStatusDesc(tr("Unavailable"), "");
        }
        else
        {
            sidebarItem->setStatusDesc(tr("Disconnected"), "");
        }
    }
}

void CPanelNetworkWidget::reload()
{

    KLOG_DEBUG() << "reload";
    for (int i = 0; i < ui->stackedWidget->count(); i++)
    {
        auto widget = ui->stackedWidget->widget(i);
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }

    ui->sidebar->clear();
    m_wiredDeviceList.clear();
    m_wirelessDeviceList.clear();

    initPage();
    ui->stackedWidget->setCurrentIndex(0);
}

QStringList CPanelNetworkWidget::subItemNameList()
{
    return m_subItemsList;
}

void CPanelNetworkWidget::setCurrentSubItem(int index)
{
    ui->sidebar->setCurrentRow(index);
    handleSideBarItemClicked(ui->sidebar->currentItem());
}

void CPanelNetworkWidget::handleThemeChanged(Kiran::PaletteType paletteType)
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

void CPanelNetworkWidget::handleSideBarItemClicked(QListWidgetItem *item)
{
    ui->stackedWidget->setCurrentIndex(item->data(Qt::UserRole).toInt());

    QString itemText = item->text();
    KLOG_DEBUG() << "item clicked:" << item->text();
    if (itemText.contains(tr("Wireless Network")))
    {
        KLOG_DEBUG() << "item clicked wireless";
        foreach (auto device , m_wirelessDeviceList)
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
}

// XXX:需要优化，改成动态增减侧边栏subItems，而不是全部重新加载
void CPanelNetworkWidget::handleDeviceAdded(const QString &devicePath)
{
    KLOG_DEBUG() << "DeviceAdded :" << devicePath;
    reload();
    emit subItemsChanged();
}

void CPanelNetworkWidget::handleDeviceRemoved(const QString &devicePath)
{
    KLOG_DEBUG() << "DeviceRemoved: " << devicePath;
    reload();
    emit subItemsChanged();
}

void CPanelNetworkWidget::handleWirelessEnabledChanged(bool enable)
{
}

void CPanelNetworkWidget::handleNetworkingEnabledChanged(bool enable)
{
}
