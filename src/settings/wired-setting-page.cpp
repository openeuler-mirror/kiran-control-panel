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
    connect(ui->returnButton, &QPushButton::clicked, [=]() { emit returnPreviousPage(); });
    connect(ui->saveButton, &QPushButton::clicked, this, &WiredSettingPage::handleSaveButtonClicked);
    connect(m_security, &QAbstractButton::toggled, [this](bool checked) {});

    connect(ui->commSetting, &CommSettingWidget::disconnect, [=]() {
        NetworkManager::deactivateConnection(m_activeConnectionPath);
    });

    connect(ui->commSetting, &CommSettingWidget::deleteConnetion, this, &WiredSettingPage::handleDeleteConnection);
}

//XXX:有待优化流程
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
        ui->commSetting->initDisconnectAndDeleteButton(SETTING_CONNECTION_STATUS_NEW);
    }
    else
    {
        KLOG_DEBUG() << "m_connectionSettings->id():" << m_connectionSettings->id();
        KLOG_DEBUG() << "m_connectionSettings->interfaceName():" << m_connectionSettings->interfaceName();

        //通过将激活路径传入SettingPage,判断该连接是否激活，也可通过uuid判断
        if (activeConnectionPath.isEmpty())
        {
            ui->commSetting->initDisconnectAndDeleteButton(SETTING_CONNECTION_STATUS_DEACTIVATED);
        }
        else
        {
            m_activeConnectionPath = activeConnectionPath;
            ui->commSetting->initDisconnectAndDeleteButton(SETTING_CONNECTION_STATUS_ACTIVATED);
        }

        ui->networkName->setText(m_connectionSettings->id());
        m_autoConnection->setChecked(m_connectionSettings->autoconnect());
        m_security->setChecked(false);

        CommConfigInfo wiredCommConfig;

        //XXX:分类设置有待优化
        //Ipv4 Address
        wiredCommConfig.ipv4Method = m_ipv4Setting->method();
        if (!m_ipv4Setting->addresses().isEmpty())
        {
            //xxx:取addresses的方式有待改进
            IpAddress ipv4Address = m_ipv4Setting->addresses().at(0);
            wiredCommConfig.ipv4Address = ipv4Address.ip().toString();
            wiredCommConfig.ipv4Netmask = ipv4Address.netmask().toString();
            wiredCommConfig.ipv4Gateway = ipv4Address.gateway().toString();
        }

        if (!m_ipv4Setting->dns().isEmpty())
        {
            wiredCommConfig.ipv4PreferredDNS = m_ipv4Setting->dns().at(0).toString();
            if (m_ipv4Setting->dns().count() >= 2)
            {
                wiredCommConfig.ipv4AlternateDNS = m_ipv4Setting->dns().at(1).toString();
            }
        }

        //Ipv6
        wiredCommConfig.ipv6Method = m_ipv6Setting->method();
        if (!m_ipv6Setting->addresses().isEmpty())
        {
            IpAddress ipv6Address = m_ipv6Setting->addresses().at(0);
            wiredCommConfig.ipv6Address = ipv6Address.ip().toString();
            wiredCommConfig.ipv6Prefix = ipv6Address.prefixLength();
            wiredCommConfig.ipv6Gateway = ipv6Address.gateway().toString();
        }
        if (!m_ipv6Setting->dns().isEmpty())
        {
            wiredCommConfig.ipv6PreferredDNS = m_ipv6Setting->dns().at(0).toString();
            if (m_ipv6Setting->dns().count() >= 2)
            {
                wiredCommConfig.ipv6AlternateDNS = m_ipv6Setting->dns().at(1).toString();
            }
        }

        m_wiredSetting = m_connectionSettings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();

        KLOG_DEBUG() << "m_wiredSetting->macAddress().toHex(':').toUpper():"
                     << m_wiredSetting->macAddress().toHex(':').toUpper();
        KLOG_DEBUG() << "m_wiredSetting->clonedMacAddress().toHex(':').toUpper():"
                     << m_wiredSetting->clonedMacAddress().toHex(':').toUpper();
        wiredCommConfig.ethernetDeviceMac = m_wiredSetting->macAddress().toHex(':').toUpper();
        wiredCommConfig.ethernetCloneDeviceMac = m_wiredSetting->clonedMacAddress().toHex(':').toUpper();

        wiredCommConfig.mtu = m_wiredSetting->mtu();

        ui->commSetting->initCommConfig(wiredCommConfig);
    }
}

//xxx:放到父类中去
void WiredSettingPage::handleSaveButtonClicked()
{
    if (m_connectionSettings.isNull())
    {
        initConnectionSettings(ConnectionSettings::ConnectionType::Wired);
        m_wiredSetting = m_connectionSettings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
        configureConnection();

        KLOG_DEBUG() << "add new connection";
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
        QDBusPendingReply<> replyUpdate = m_connection->update(m_connectionSettings->toMap());
        replyUpdate.waitForFinished();
        if (replyUpdate.isError())
        {
            KLOG_DEBUG() << "error occurred while updating the connection" << replyUpdate.error();
        }
    }

    emit settingChanged();
}

//TODO:判断输入格式合法性
void WiredSettingPage::configureConnection()
{
    m_connectionSettings->setId(ui->networkName->text());
    m_connectionSettings->setAutoconnect(m_autoConnection->isChecked());

    CommConfigInfo configInfo = ui->commSetting->getCommConfig();
    setIpv4Settings(configInfo);
    setIpv6Settings(configInfo);
    setWiredSettings(configInfo);
}

void WiredSettingPage::setIpv6Settings(CommConfigInfo &configInfo)
{
    if (configInfo.ipv6Method == Ipv6Setting::ConfigMethod::Ignored)
    {
        m_ipv6Setting->setAddresses(QList<NetworkManager::IpAddress>());
        return;
    }
    else if (configInfo.ipv6Method == Ipv6Setting::ConfigMethod::Automatic)
    {
        configInfo.ipv6Address = "";
        configInfo.ipv6Prefix = 0;
        configInfo.ipv6Gateway = "";
    }
    else
    {
    }

    m_ipv6Setting->setMethod(configInfo.ipv6Method);
    IpAddress ipv6Address;
    ipv6Address.setIp(QHostAddress(configInfo.ipv6Address));
    ipv6Address.setPrefixLength(configInfo.ipv6Prefix);
    ipv6Address.setGateway(QHostAddress(configInfo.ipv6Gateway));

    QList<IpAddress> ipv6AddressList;
    ipv6AddressList << ipv6Address;
    m_ipv6Setting->setAddresses(ipv6AddressList);

    QList<QHostAddress> ipv6DNS;
    ipv6DNS << QHostAddress(configInfo.ipv6PreferredDNS) << QHostAddress(configInfo.ipv6AlternateDNS);
    m_ipv6Setting->setDns(ipv6DNS);
}
