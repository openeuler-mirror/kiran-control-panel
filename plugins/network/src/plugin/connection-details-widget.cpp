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
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#include "connection-details-widget.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>
#include "ui_connection-details-widget.h"

ConnectionDetailsWidget::ConnectionDetailsWidget(Device::Ptr device, QWidget *parent)
    : QWidget(parent), ui(new Ui::ConnectionDetailsWidget)
{
    ui->setupUi(this);
    m_device = device;
    initUI();
}

ConnectionDetailsWidget::~ConnectionDetailsWidget()
{
    delete ui;
}

void ConnectionDetailsWidget::initUI()
{
    setFrameBroder(false);
    ui->ipv6GatewayWidget->setVisible(false);
    m_activeConnection = m_device->activeConnection();
    if (m_activeConnection == nullptr)
        return;
    m_connection = m_activeConnection->connection();

    ui->networkInterface->setText(m_device->interfaceName());
    if (m_activeConnection->type() == ConnectionSettings::Wireless)
        setWirelessSpecificDetails();
    else
    {
        ui->wirelssSpecialWidget->setVisible(false);
        WiredDevice::Ptr wiredDevice = qobject_cast<WiredDevice *>(m_device);
        ui->mac->setText(wiredDevice->hardwareAddress());
        int bitRate = wiredDevice->bitRate();
        QString rate = "-";
        if (bitRate != 0)
            rate = QString("%1 Mb/s").arg(bitRate / 1000);
        ui->rate->setText(rate);
    }
    setIpDetails();
}

void ConnectionDetailsWidget::setWirelessSpecificDetails()
{
    ui->wirelssSpecialWidget->setVisible(true);
    WirelessSetting::Ptr wirelessSetting = m_connection->settings()->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    WirelessDevice::Ptr wirelessDevice = qobject_cast<WirelessDevice *>(m_device);
    AccessPoint::Ptr activeAccessPoint = wirelessDevice->activeAccessPoint();

    const char *securityType = "None";
    int bitRate, frequency;
    if (activeAccessPoint != nullptr)
    {
        AccessPoint::Capabilities capabilities = activeAccessPoint->capabilities();
        AccessPoint::WpaFlags wpaFlags = activeAccessPoint->wpaFlags();
        AccessPoint::WpaFlags rsnFlags = activeAccessPoint->rsnFlags();

        if (capabilities.testFlag(AccessPoint::Capability::Privacy) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
        {
            securityType = "Wep";
        }
        if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk))
        {
            securityType = "WPA/WPA2 Personal";
        }
        if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x))
        {
            securityType = "WPA/WPA2 Enterprise";
        }

        bitRate = activeAccessPoint->maxBitRate();
        frequency = activeAccessPoint->frequency();
    }
    ui->securityType->setText(tr(securityType));

    WirelessSetting::FrequencyBand band = NetworkManager::findFrequencyBand(frequency);
    QString frequencyBand = band == WirelessSetting::FrequencyBand::A ? "5G" : (band == WirelessSetting::FrequencyBand::Bg ? "2.4G" : "Automatic");
    ui->frequencyBand->setText(frequencyBand);

    int channel = NetworkManager::findChannel(frequency);
    ui->channel->setText(QString::number(channel));
    ui->mac->setText(wirelessDevice->hardwareAddress());

    QString rate = "-";
    if (bitRate != 0)
        rate = QString("%1 Mb/s").arg(bitRate / 1000);
    ui->rate->setText(rate);
}

void ConnectionDetailsWidget::setIpDetails()
{
    IpConfig ipV4Config = m_activeConnection->ipV4Config();
    IpAddress ipv4Address = ipV4Config.addresses().at(0);
    QString address = ipv4Address.ip().toString();
    QString netmask = ipv4Address.netmask().toString();
    QString gateway = ipV4Config.gateway();
    ui->ipv4->setText(address);
    ui->subnetMask->setText(netmask);
    ui->ipv4Gateway->setText(gateway);

    QString preferredDNS = "-";
    Dhcp4Config::Ptr dhcp = m_activeConnection->dhcp4Config();
    auto dhcpOptions = dhcp->options();
    if (!dhcpOptions.isEmpty())
    {
        QVariant domainNameServers = dhcpOptions.value("domain_name_servers");
        QStringList dns = domainNameServers.toString().split(" ");
        preferredDNS = dns.value(0);
    }
    else
    {
        Ipv4Setting::Ptr ipv4Setting = m_connection->settings()->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
        if (!ipv4Setting->dns().isEmpty())
        {
            preferredDNS = ipv4Setting->dns().at(0).toString();
        }
    }
    ui->preferredDNS->setText(preferredDNS);

    IpConfig ipV6Config = m_activeConnection->ipV6Config();
    IpAddress ipv6Address = ipV6Config.addresses().at(0);
    QString ipv6 = ipv6Address.ip().toString();
    int prefix = ipv6Address.prefixLength();
    ui->ipv6->setText(ipv6);
    ui->prefix->setText(QString::number(prefix));
}

void ConnectionDetailsWidget::setFrameBroder(bool enable)
{
    ui->securityTypeWidget->setDrawBroder(enable);
    ui->frequencyBandWidget->setDrawBroder(enable);
    ui->channelWidget->setDrawBroder(enable);
    ui->InterfaceWidget->setDrawBroder(enable);
    ui->macWidget->setDrawBroder(enable);
    ui->ipv4Widget->setDrawBroder(enable);
    ui->gatewayWidget->setDrawBroder(enable);
    ui->preferredDNSWidget->setDrawBroder(enable);
    ui->subnetMaskWidget->setDrawBroder(enable);
    ui->ipv6Widget->setDrawBroder(enable);
    ui->ipv6GatewayWidget->setDrawBroder(enable);
    ui->prefixWidget->setDrawBroder(enable);
    ui->rateWidget->setDrawBroder(enable);
}
