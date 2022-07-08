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

#include "cpanel-network-widget.h"
#include <qt5-log-i.h>
#include "plugin/details-page.h"
#include "ui_cpanel-network-widget.h"
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
    initSubItemsList();
    initPage();
    initConnect();

}

void CPanelNetworkWidget::initPage()
{
    int row = 0;
    for (int i = 0; i < m_wiredDeviceList.count(); ++i)
    {
        auto device = m_wiredDeviceList.value(i);
        QString devicePath = device->uni();
        QString deviceName = device->interfaceName();
        WiredManager *wiredManager = new WiredManager(devicePath, this);
        ui->stackedWidget->insertWidget(row,wiredManager);

        QString subItemName = tr("Wired Connection %1");
        QString subItemNameStr = subItemName.arg(i + 1);
        if(m_wiredDeviceList.count() == 0)
            ui->sidebar->insertItem(row, tr("Wired Connection"));
        else
            ui->sidebar->insertItem(row, subItemNameStr);
        ui->sidebar->item(row)->setData(Qt::UserRole, row);
        ui->sidebar->item(row)->setIcon(trayIconColorSwitch(":/kcp-network-images/wired.svg"));
        row++;
    }

    for (int i = 0; i < m_wirelessDeviceList.count(); ++i)
    {
        auto device = m_wirelessDeviceList.value(i);
        QString devicePath = device->uni();
        QString deviceName = device->interfaceName();
        WirelessManager *wirelessManager = new WirelessManager(devicePath, this);
        ui->stackedWidget->insertWidget(row,wirelessManager);

        QString subItemName = tr("Wireless Connection %1");
        QString subItemNameStr = subItemName.arg(i+1);
        if(m_wirelessDeviceList.count() == 0)
            ui->sidebar->insertItem(row, tr("Wireless Connection"));
        else
            ui->sidebar->insertItem(row, subItemNameStr);

        ui->sidebar->item(row)->setData(Qt::UserRole, row);
        ui->sidebar->item(row)->setIcon(trayIconColorSwitch(":/kcp-network-images/wireless.svg"));
        row++;
    }

    VpnManager *vpnManager = new VpnManager(this);
    ui->stackedWidget->insertWidget(row,vpnManager);

    ui->sidebar->insertItem(row, tr("VPN"));
    ui->sidebar->item(row)->setData(Qt::UserRole, row);
    ui->sidebar->item(row)->setIcon(trayIconColorSwitch(":/kcp-network-images/vpn.svg"));
    row++;

    DetailsPage *networkDetails = new DetailsPage(this);
    ui->stackedWidget->insertWidget(row,networkDetails);
    ui->sidebar->insertItem(row, tr("Network Details"));
    ui->sidebar->item(row)->setData(Qt::UserRole, row);
    ui->sidebar->item(row)->setIcon(trayIconColorSwitch(":/kcp-network-images/network-details.svg"));
    row++;
}

void CPanelNetworkWidget::getAvailableDeviceList()
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        if (dev->state() == Device::Unavailable)
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

void CPanelNetworkWidget::initSubItemsList()
{
    getAvailableDeviceList();

    if (m_wiredDeviceList.count() > 1)
    {
        QString subItemName = tr("Wired Connection %1");
        for (int i = 0; i < m_wiredDeviceList.count(); ++i)
        {
            QString subItemNameStr = subItemName.arg(i + 1);
        }
    }
    else if (m_wiredDeviceList.count() == 1)
    {
        m_subItemsList << tr("Wired Connection");
    }
    else  // m_wiredDeviceList.count() == 0
    {
    }

    if (m_wirelessDeviceList.count() > 1)
    {
        QString subItemName = tr("Wireless Connection %1");
        for (int i = 0; i < m_wirelessDeviceList.count(); ++i)
        {
            QString subItemNameStr = subItemName.arg(i + 1);
        }
    }
    else if (m_wirelessDeviceList.count() == 1)
    {
        m_subItemsList << tr("Wireless Connection");
    }
    else  // m_wiredDeviceList.count() == 0
    {
    }

    m_subItemsList << tr("VPN")
                   << tr("Network Details");
}

void CPanelNetworkWidget::initConnect()
{
    m_Timer.setInterval(500);
    m_Timer.setSingleShot(true);
    waitCounts = 1;
    connect(&m_Timer, &QTimer::timeout, [=]()
            {
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
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
                waitCounts++;
                if(waitCounts > MAX_WAIT_COUNTS)
                {
                    KLOG_INFO() << "This device is not currently managed by NetworkManager";
                    m_Timer.stop();
                } });

    // Note:新设备插入后，需要等待一段时间，Device::List networkInterfaces() 来不及更新
    connect(notifier(), &Notifier::deviceAdded, [=](const QString &uni)
            {
                m_addDevicePath = uni;
                Device::Ptr device = findNetworkInterface(m_addDevicePath);
                KLOG_DEBUG() << "device->availableConnections().isEmpty():" << device->availableConnections().isEmpty();
                KLOG_DEBUG() << "device->managed():" << device->managed();
                if(!device->isValid())
                {
                    KLOG_INFO() << "this device interface is invalid!";
                    return ;
                }
                if(device->managed())
                    handleDeviceAdded(m_addDevicePath);
                else
                {
                    KLOG_INFO() << "this device interface is not ready";
                    m_Timer.start();
                    KLOG_INFO() << "wait counts:" << waitCounts;
                } });

    connect(notifier(), &Notifier::deviceRemoved, this, &CPanelNetworkWidget::handleDeviceRemoved);

    connect(ui->sidebar, &QListWidget::itemClicked, [this](QListWidgetItem *item)
            {
                KLOG_DEBUG() << " item->data(Qt::UserRole).toInt():" << item->data(Qt::UserRole).toInt();
                ui->stackedWidget->setCurrentIndex(item->data(Qt::UserRole).toInt());
            });

    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &CPanelNetworkWidget::handleThemeChanged);
}

// XXX:暂时全部item重新加载有待优化
void CPanelNetworkWidget::handleDeviceAdded(const QString &devicePath)
{
    reload();
    emit subItemsChanged();
}

void CPanelNetworkWidget::handleDeviceRemoved(const QString &devicePath)
{
    reload();
    emit subItemsChanged();
}

void CPanelNetworkWidget::reload()
{
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
    {
        auto widget = ui->stackedWidget->widget(i);
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }

    ui->sidebar->clear();
    m_wiredDeviceList.clear();
    m_wirelessDeviceList.clear();
    m_subItemsList.clear();

    initSubItemsList();
    initPage();
    ui->stackedWidget->setCurrentIndex(0);
}

QPixmap CPanelNetworkWidget::trayIconColorSwitch(const QString &iconPath)
{
    //icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(16,16);
    if( Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK )
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}

void CPanelNetworkWidget::handleThemeChanged(Kiran::PaletteType paletteType)
{
    for (int i = 0; i < ui->sidebar->count(); ++i)
    {
        auto icon = ui->sidebar->item(i)->icon();
        QPixmap pixmap = icon.pixmap(16,16);
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
        ui->sidebar->item(i)->setIcon(pixmap);
    }
}
