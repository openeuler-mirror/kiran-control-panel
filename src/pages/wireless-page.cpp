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
#include "wireless-page.h"
#include <qt5-log-i.h>
#include <QScrollBar>
#include "ui_wireless-page.h"
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
WirelessPage::WirelessPage(QWidget *parent) : Page(parent), ui(new Ui::WirelessPage)
{
    ui->setupUi(this);
    getDeviceInfo(Device::Wifi);
    initUI();
    initConnection();
    handleRequestActivateConnection("");
}

WirelessPage::~WirelessPage()
{
    delete ui;
}

void WirelessPage::initUI()
{
    ui->connectionShowPage->setTitle(tr("Wireless Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
    ui->connectionShowPage->showConnectionLists(ConnectionSettings::Wireless);
}

void WirelessPage::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    //    connect(ui->returnButton, &QPushButton::clicked, this, &WirelessPage::handleReturnPreviousPage);

    connect(ui->returnButton, &QPushButton::clicked, [=]() {
        ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
    });
}

//如何获取到connectionPath,暂时考虑连接的时候增加一个wirelessSetting，然后再激活
void WirelessPage::handleRequestActivateConnection(QString connectionPath)
{
    NetworkManager::ConnectionSettings *settings = new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless);
    const Device::List deviceList = NetworkManager::networkInterfaces();
    WirelessDevice::Ptr wifiDevice;

    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == Device::Wifi)
        {
            wifiDevice = qobject_cast<WirelessDevice *>(dev);
            break;
        }
    }

    const QStringList accessPointList = wifiDevice->accessPoints();
    KLOG_DEBUG() <<  "accessPointList:" << accessPointList;
    QString ssid;
    QString result;
    QString accessPointPath;

    ssid = "科技园";
    for (const QString &ap : accessPointList)
    {
        AccessPoint accessPoint(ap);
        KLOG_DEBUG() << "accessPoint.ssid():" << accessPoint.ssid();
        KLOG_DEBUG() << "accessPoint.uni():" << accessPoint.uni();
        KLOG_DEBUG() << "accessPoint.wpaFlags():" << accessPoint.wpaFlags();
        if(accessPoint.ssid() == ssid && accessPoint.signalStrength() >= 80)
        {
            accessPointPath =ap;
        }
    }


    settings->setId("testWireless22222");
    settings->setUuid(QUuid::createUuid().toString().mid(1, QUuid::createUuid().toString().length() - 2));

    NetworkManager::WirelessSetting::Ptr wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wirelessSetting->setSsid(ssid.toUtf8());

    NetworkManager::Ipv4Setting::Ptr ipv4Setting = settings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    NetworkManager::WirelessSecuritySetting::Ptr wifiSecurity =
        settings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
    wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
    wifiSecurity->setPsk("12345678");
    wifiSecurity->setInitialized(true);
    wirelessSetting->setSecurity("802-11-wireless-security");

    KLOG_DEBUG() <<  "wifiDevice->uni():" << wifiDevice->uni();
    KLOG_DEBUG() << "accessPointPath:" << accessPointPath;

    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply = NetworkManager::activateConnection("", wifiDevice->uni(), accessPointPath);
//    QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply = NetworkManager::addAndActivateConnection(settings->toMap(), wifiDevice->uni(), accessPointPath);

    reply.waitForFinished();

    if (reply.isValid())
    {

    }
    else
    {
        KLOG_DEBUG() << "Connection failed: " << reply.error();
    }
}

void WirelessPage::handleReturnPreviousPage()
{

}
