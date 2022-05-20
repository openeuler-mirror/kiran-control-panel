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

#include "tray-page.h"
#include "ui_tray-page.h"
#include <qt5-log-i.h>
#include "wired-tray-widget.h"
TrayPage::TrayPage(TrayConnectionType trayConnectionType, QWidget *parent) : QWidget(parent), ui(new Ui::TrayPage)
{
    ui->setupUi(this);
    m_trayConnectionType = trayConnectionType;
    init();
}

TrayPage::~TrayPage()
{
    delete ui;
}

void TrayPage::init()
{
    initUI();
    initConnection();
}

void TrayPage::initUI()
{

    if (m_trayConnectionType == TRAY_CONNECTION_TYPE_WIRED)
    {
        initWiredPage();
    }
    else
    {
        initWirelessPage();
    }
}

void TrayPage::initConnection()
{

}

void TrayPage::initWiredPage()
{
    ui->deviceLabel->setText(tr("Select wired network card"));
    getDeviceList(Device::Ethernet);

    if(m_deviceList.count() > 1)
    {
        setMultiWiredDeviceWidget();
    }
    else if(m_deviceList.count() == 1)
    {
        ui->selectDevicewidget->setVisible(false);
        QString devicePath =  m_deviceList.at(0)->uni();
        KLOG_DEBUG() << "initWiredPage devicePath:" << devicePath;
        WiredTrayWidget *wiredTrayWidget = new WiredTrayWidget(devicePath,this);
        ui->stackedWidget->addWidget(wiredTrayWidget);
    }
    else
    {
        //m_deviceeList.count == 0
    }
}

void TrayPage::initWirelessPage()
{
    ui->deviceLabel->setText(tr("Select wireless network card"));
    getDeviceList(Device::Wifi);
}

void TrayPage::getDeviceList(Device::Type deviceType)
{
    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == deviceType)
        {
            m_deviceList << dev;
        }
    }
    KLOG_INFO() << "m_deviceList:" << m_deviceList;
    if (m_deviceList.isEmpty())
    {
        KLOG_INFO() << "No available devices were found";
    }
}

void TrayPage::setMultiWiredDeviceWidget()
{
    for (Device::Ptr dev : m_deviceList)
    {
        QSharedPointer<WiredDevice> wiredDevice = qobject_cast<WiredDevice*>(dev);
        QString devicePath = wiredDevice->uni();
        QString deviceName = wiredDevice->interfaceName();
        ui->deviceComboBox->addItem(deviceName,devicePath);

        WiredTrayWidget *wiredTrayWidget = new WiredTrayWidget(devicePath,this);
        ui->stackedWidget->addWidget(wiredTrayWidget);

    }
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                QString devicePath = ui->deviceComboBox->currentData().toString();
                handleDeviceComboBoxChanged(index);
    });
    ui->selectDevicewidget->setVisible(true);
}

void TrayPage::handleDeviceComboBoxChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

//重新获取device、初始化，刷新
/*void TrayPage::handleDeviceAdded(const QString &devicePath)
{
    //需要 等待一段时间
    Device::Ptr device = findNetworkInterface(devicePath);
    if(device->type() == Device::Ethernet)
    {
        int previousWidgetCount = ui->stackedWidget->count();
        getDeviceList(Device::Ethernet);
        if(previousWidgetCount != 0)
        {
            ui->deviceComboBox->clear();
            for (int i = 0; i < previousWidgetCount; ++i)
            {
                QWidget *widget= ui->stackedWidget->widget(i);
                ui->stackedWidget->removeWidget(widget);
                widget->deleteLater();
            }
            setMultiWiredDeviceWidget();
        }
        else
            ui->selectDevicewidget->setVisible(false);
    }
    else if(device->type() == Device::Wifi)
    {

    }
}

void TrayPage::handleDeviceRemoved(const QString &devicePath)
{

}*/
QStringList TrayPage::devicePathList()
{
    QStringList devicePathList;
    for (Device::Ptr device :m_deviceList)
    {
        devicePathList << device->uni();
    }
    return devicePathList;
}

int TrayPage::pageHeight()
{
    KLOG_DEBUG() << "ui->selectDevicewidget->height():" << ui->selectDevicewidget->height();
    KLOG_DEBUG() << "ui->stackedWidget->height():" << ui->stackedWidget->height();
    ui->stackedWidget->widget(0);

    int height = ui->selectDevicewidget->height() + ui->stackedWidget->height();
    return height;
}
