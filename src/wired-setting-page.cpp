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

#include "wired-setting-page.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include "ui_wired-setting-page.h"

WiredSettingPage::WiredSettingPage(QWidget *parent) : SettingPage(parent), ui(new Ui::WiredSettingPage)
{
    ui->setupUi(this);
    init();
    KLOG_DEBUG() << "WiredSettingPage::WiredSettingPage(QWidget *parent)";
}

WiredSettingPage::~WiredSettingPage()
{
    KLOG_DEBUG() << "WiredSettingPage::~WiredSettingPage()";
    delete ui;
}

void WiredSettingPage::init()
{
    initUI();
    initConnecton();

    const Device::List deviceList = networkInterfaces();
    for (Device::Ptr dev : deviceList)
    {
        if (dev->type() == Device::Ethernet)
        {
            m_wiredDevice = qobject_cast<WiredDevice *>(dev);
            break;
        }
    }

    if (!m_wiredDevice)
    {
        KLOG_DEBUG() << "Wired device not found";
    }
}

void WiredSettingPage::initUI()
{
    m_autoConnection = new KiranSwitchButton(this);
    m_security = new KiranSwitchButton(this);
    ui->autoConnectionLayout->addWidget(m_autoConnection);
    ui->securityLayout->addWidget(m_security);
}

void WiredSettingPage::initConnecton()
{
    connect(ui->returnButton, &QPushButton::clicked, [=]() {
        emit returnPreviousPage();
    });

    connect(ui->saveButton, &QPushButton::clicked, this, &WiredSettingPage::handleSaveButtonClicked);
    connect(m_security, &QAbstractButton::toggled, [this](bool checked) {});

    //    connect(ui->disconnectButton, &QPushButton::clicked, [=]() {
    //        KLOG_DEBUG() << "NetworkManager::activeConnectionsPaths():" << NetworkManager::activeConnectionsPaths();
    ////        QString activeConnectionPath = "";
    ////        NetworkManager::deactivateConnection(activeConnectionPath);
    //    });
    //    connect(ui->deleteButton, &QPushButton::clicked, [=]() {
    //        m_connection->remove();
    //        emit settingChanged();
    //    });
}

void WiredSettingPage::refreshSettingPage(QString activeConnectionPath)
{
    if (m_connectionSettings.isNull())
    {
        //生成名称数字后缀
        QString connectionName = tr("Wired Connection %1");
        ui->networkName->setText(connectionName.arg(connectionSuffixNum(connectionName, ConnectionSettings::ConnectionType::Wired)));
        m_autoConnection->setChecked(true);
        m_security->setChecked(false);
        //XXX:默认设置的初始化可以改进
        CommConfigInfo defaultCommConfig;
        ui->commSetting->initCommConfig(defaultCommConfig);
    }
    else
    {
        KLOG_DEBUG() << "m_connectionSettings.isNull(): " << m_connectionSettings.isNull();
        KLOG_DEBUG() << "m_connectionSettings:" << m_connectionSettings;
        KLOG_DEBUG() << "m_connectionSettings->id():" << m_connectionSettings->id();
        KLOG_DEBUG() << "m_connectionSettings->interfaceName():" << m_connectionSettings->interfaceName();
        KLOG_DEBUG() << "m_connectionSettings->autoconnect():" << m_connectionSettings->autoconnect();

        //通过激活路径判断该连接是否激活，也可通过uuid判断
        if(activeConnectionPath.isEmpty())
        {

        }
        else
        {
            m_activeConnectionPath = activeConnectionPath;
        }



        ui->networkName->setText(m_connectionSettings->id());
        m_autoConnection->setChecked(m_connectionSettings->autoconnect());
        m_security->setChecked(false);

        m_wiredCommConfig.ipv4Method = m_ipv4Setting->method();
        if (!m_ipv4Setting->addresses().isEmpty())
        {
            //xxx:取addresses的方式有待改进
            IpAddress ipv4Address = m_ipv4Setting->addresses().at(0);
            m_wiredCommConfig.ipv4Address = ipv4Address.ip().toString();
            m_wiredCommConfig.ipv4Netmask = ipv4Address.netmask().toString();
            m_wiredCommConfig.ipv4Gateway = ipv4Address.gateway().toString();
        }
        ui->commSetting->initCommConfig(m_wiredCommConfig);

    }
}

void WiredSettingPage::handleSaveButtonClicked()
{
    if (m_connectionSettings.isNull())
    {
        initConnectionSettings(ConnectionSettings::ConnectionType::Wired);
        configureConnection();
        KLOG_DEBUG() << "add new connection";
        KLOG_DEBUG() << "m_connectionSettings->toMap():" << m_connectionSettings->toMap();
        QDBusPendingReply<QDBusObjectPath> replyAdd = NetworkManager::addConnection(m_connectionSettings->toMap());
        replyAdd.waitForFinished();
        if (replyAdd.isError())
        {
            KLOG_DEBUG() << "add connection failed," << replyAdd.error();
        }
    }
    else
    {
        configureConnection();
        KLOG_DEBUG() << "m_connectionSettings->toMap(): " << m_connectionSettings->toMap();
        QDBusPendingReply<> replyUpdate = m_connection->update(m_connectionSettings->toMap());
        replyUpdate.waitForFinished();
        if (replyUpdate.isError())
        {
            KLOG_DEBUG() << "error occurred while updating the connection" << replyUpdate.error();
        }
    }

    emit settingChanged();

    //            KLOG_DEBUG() << "m_connection->path():" << m_connection->path();
    //                QDBusPendingReply<QDBusObjectPath> reply =
    //                    NetworkManager::activateConnection(m_connection->path(),m_wiredDevice->uni(),"");
}

void WiredSettingPage::configureConnection()
{
    CommConfigInfo configInfo = ui->commSetting->getCommConfig();
    if (configInfo.ipv4Method == Ipv4Setting::ConfigMethod::Automatic)
    {
        configInfo.ipv4Address = "";
        configInfo.ipv4Netmask = "";
        configInfo.ipv4Gateway = "";
    }
    else
    {
        //TODO:判断输入格式合法性
    }

    KLOG_DEBUG() << "configInfo.ipv4Address:" << configInfo.ipv4Address;
    KLOG_DEBUG() << "configInfo.ipv4Netmask:" << configInfo.ipv4Netmask;
    KLOG_DEBUG() << "configInfo.ipv4Gateway:" << configInfo.ipv4Gateway;

    //    m_connectionSettings->setInterfaceName("test_interfaceName");
    m_connectionSettings->setId(ui->networkName->text());
    m_connectionSettings->setAutoconnect(m_autoConnection->isChecked());

    m_ipv4Setting->setMethod(configInfo.ipv4Method);

    IpAddress ipv4AddressSetting;
    ipv4AddressSetting.setIp(QHostAddress(configInfo.ipv4Address));
    ipv4AddressSetting.setNetmask(QHostAddress(configInfo.ipv4Netmask));
    ipv4AddressSetting.setGateway(QHostAddress(configInfo.ipv4Gateway));

    QList<IpAddress> ipv4Addresses;
    ipv4Addresses << ipv4AddressSetting;
    m_ipv4Setting->setAddresses(ipv4Addresses);
}

