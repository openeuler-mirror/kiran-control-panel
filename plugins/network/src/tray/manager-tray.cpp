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

#include "manager-tray.h"
#include <qt5-log-i.h>
#include <QVBoxLayout>
#include "status-notifier-manager.h"
#include "tray-page.h"
#include "wired-tray-widget.h"
#include "wireless-tray-widget.h"
#include <QMenu>

#define STATUS_NOTIFIER_MANAGER "org.kde.StatusNotifierManager"
#define STATUS_NOTIFIER_MANAGER_OBJECT_NAME "/StatusNotifierManager"
#define MAX_WAIT_COUNTS 10

ManagerTray::ManagerTray(QWidget *parent) : QWidget(parent),
                                            m_wiredTrayPage(nullptr),
                                            m_wirelessTrayPage(nullptr)
{
    init();
}

ManagerTray::~ManagerTray()
{
}

void ManagerTray::init()
{
    m_statusNotifierManager = new StatusNotifierManagerInterface(STATUS_NOTIFIER_MANAGER, STATUS_NOTIFIER_MANAGER_OBJECT_NAME, QDBusConnection::sessionBus(), this);
    initUI();
    initMenu();
    initConnect();
}

void ManagerTray::initUI()
{
    initTrayPage();
    initTrayIcon();
    m_verticalLayout = new QVBoxLayout(this);
    if(m_wiredTrayPage != nullptr)
        m_verticalLayout->addWidget(m_wiredTrayPage);
    if(m_wirelessTrayPage != nullptr)
        m_verticalLayout->addWidget(m_wirelessTrayPage);
    m_verticalLayout->setMargin(0);
    m_verticalLayout->setSpacing(0);
    this->setWindowFlags(Qt::Popup | Qt::BypassWindowManagerHint);
}

void ManagerTray::initConnect()
{
    connect(m_systemTray, &QSystemTrayIcon::activated, this,&ManagerTray::handleTrayClicked);

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

    // 需要等待一段时间，Device::List networkInterfaces() 来不及更新
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

    connect(notifier(), &Notifier::deviceRemoved, this,&ManagerTray::handleDeviceRemoved);
    connect(notifier(),&Notifier::statusChanged,this,&ManagerTray::handleNetworkManagerStatusChanged);
}

void ManagerTray::initTrayIcon()
{
    m_systemTray = new QSystemTrayIcon();
    setTrayIcon(NetworkManager::status());
    m_systemTray->show();
}

void ManagerTray::initMenu()
{
    m_menu = new QMenu(this);
    m_networkSetting = new QAction(tr("Network settings"));

    m_menu->addAction(m_networkSetting);
    getTrayGeometry();
    m_menu->sizeHint();
//    m_menu->exec();
//    m_menu->popup(QPoint(500,500));
    m_systemTray->setContextMenu(m_menu);
    connect(m_networkSetting,&QAction::triggered,this,&ManagerTray::handleNetworkSettingClicked);
}

//初始化条件：设备存在且可用
void ManagerTray::initTrayPage()
{
    getAvailableDeviceList();
    if(m_wiredDeviceList.count() != 0)
        m_wiredTrayPage = new TrayPage(m_wiredDeviceList, this);
    if(m_wirelessDeviceList.count() != 0)
        m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList,this);
}

void ManagerTray::getAvailableDeviceList()
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        KLOG_DEBUG() << "dev->interfaceName():" << dev->interfaceName();
        KLOG_DEBUG() << "dev->managed():" << dev->managed();
        KLOG_DEBUG() << "dev->availableConnections():" << dev->availableConnections();
        KLOG_DEBUG() << "dev->state():" << dev->state();

        if(dev->state() == Device::Unavailable)
            continue ;

        switch (dev->type())
        {
        case Device::Ethernet:
            m_wiredDeviceList << dev;
            break ;
        case Device::Wifi:
            m_wirelessDeviceList << dev;
            break ;
        default:
            break ;
        }
    }
}


void ManagerTray::handleTrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        showTrayPage();
        break;
    default:
        break;
    }
}

void ManagerTray::handleNetworkSettingClicked()
{
    QProcess process(this);
    process.startDetached("kiran-control-panel");
    KLOG_DEBUG() << "handleNetworkSettingClicked";
}

void ManagerTray::showTrayPage()
{
    KLOG_DEBUG() << "showTrayPage";
    setTrayPagePos();
    this->show();
}

void ManagerTray::setTrayPagePos()
{
    getTrayGeometry();
    int pageWidth = 240;
    // TODO:随插入项高度动态变化
    int maxHeight = 434;
    int pageHeight = this->sizeHint().height();
    KLOG_DEBUG() << "pageHeight:" << pageHeight;
    this->setGeometry(m_xTray - pageWidth / 2, m_yTray - pageHeight, pageWidth, pageHeight);
}

void ManagerTray::getTrayGeometry()
{
    // 名称待修改
    QDBusPendingReply<QString> getGeometry = m_statusNotifierManager->GetGeometry("kiran-cpanel-network");
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

//TODO:切换图标颜色
void ManagerTray::setTrayIcon(NetworkManager::Status status)
{
    //判断连接为有线还是无线，如果同时存在则图标为无线
    switch (status)
    {
    case NetworkManager::Status::Unknown:
        // 未知
        m_systemTray->setIcon(QIcon(":/kcp-network-images/wired-error.svg"));
        break ;
    case NetworkManager::Status::Asleep:
        // 不可用
        m_systemTray->setIcon(QIcon(":/kcp-network-images/wired-disconnected.svg"));
        break ;
    case NetworkManager::Status::Disconnected:
        // 不可用
        m_systemTray->setIcon(QIcon(":/kcp-network-images/wired-disconnected.svg"));
        break ;
    case NetworkManager::Status::Disconnecting:
        break ;
    case NetworkManager::Status::Connecting:
        // 加载动画
        break ;
    case NetworkManager::Status::ConnectedLinkLocal:
        m_systemTray->setIcon(QIcon(":/kcp-network-images/wired-disconnected.svg"));
        // 不可用
        break ;
    case NetworkManager::Status::ConnectedSiteOnly:
        // 未知
        m_systemTray->setIcon(QIcon(":/kcp-network-images/wired-error.svg"));
        break ;
    case NetworkManager::Status::Connected:
        // 可用
        m_systemTray->setIcon(QIcon(":/kcp-network-images/wired-connection.svg"));
        break ;
    default:
        break ;
    }
}

// 重新获取device、初始化，刷新
//XXX:可以优化
void ManagerTray::handleDeviceAdded(const QString &devicePath)
{
    Device::Ptr device = findNetworkInterface(devicePath);
    if (device->type() == Device::Ethernet)
    {
        reloadWiredTrayPage();

        /*
        KLOG_DEBUG() << "m_wiredTrayPage->sizeHint():" << m_wiredTrayPage->sizeHint();
        KLOG_DEBUG() << "m_wiredTrayPage->geometry():" << m_wiredTrayPage->geometry();
        KLOG_DEBUG() << "m_wiredTrayPage->height():" << m_wiredTrayPage->height();
        sizeIncrement();
        int pageWidth = 240;
        int pageHeight = 226;
        QSize size(pageWidth,226);
        m_wiredTrayPage->pageHeight();
        KLOG_DEBUG() << "m_wiredTrayPage->pageHeight():" << m_wiredTrayPage->pageHeight();
        pageHeight = m_wiredTrayPage->pageHeight();
//        resize(size);
//        setGeometry(1725,1040,240,226);
        setGeometry(m_xTray - pageWidth / 2, m_yTray - pageHeight, pageWidth, pageHeight);
//        showTrayPage();
        */
    }
    else if (device->type() == Device::Wifi)
    {
        reloadWirelessTrayPage();
    }
}

// XXX:当device被移除时，由于设备对象可能已经被删除，所以并不能通过findNetworkInterface(path)找到该设备接口，进而知道被删除的设备类型
void ManagerTray::handleDeviceRemoved(const QString &devicePath)
{
    if (m_wiredTrayPage->devicePathList().contains(devicePath))
        reloadWiredTrayPage();
    else if (m_wirelessTrayPage->devicePathList().contains(devicePath))
        reloadWirelessTrayPage();
}

void ManagerTray::handleNetworkManagerStatusChanged(NetworkManager::Status status)
{
    setTrayIcon(status);
}

void ManagerTray::reloadWiredTrayPage()
{
    m_verticalLayout->removeWidget(m_wiredTrayPage);
    delete m_wiredTrayPage;
    m_wiredTrayPage = nullptr;
    m_wiredDeviceList.clear();
    getAvailableDeviceList();
    if(m_wiredDeviceList.count() != 0)
        m_wiredTrayPage = new TrayPage(m_wiredDeviceList, this);

    m_verticalLayout->count();
    //XXX:待修改，使有线widget一直在最上层
    m_verticalLayout->insertWidget(0,m_wiredTrayPage);
    m_verticalLayout->setMargin(0);
    update();
}

void ManagerTray::reloadWirelessTrayPage()
{
    m_verticalLayout->removeWidget(m_wirelessTrayPage);
    delete m_wirelessTrayPage;
    m_wirelessTrayPage = nullptr;
    m_wirelessDeviceList.clear();

    getAvailableDeviceList();
    if(m_wirelessDeviceList.count() != 0)
        m_wirelessTrayPage = new TrayPage(m_wirelessDeviceList, this);
    m_verticalLayout->insertWidget(-1,m_wirelessTrayPage);
    m_verticalLayout->setMargin(0);
    update();
}


