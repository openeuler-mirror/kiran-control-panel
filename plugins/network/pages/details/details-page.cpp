/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#include "details-page.h"
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Security8021xSetting>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>
#include <QScrollBar>
#include "connection-manager.h"
#include "logging-category.h"
#include "ui_details-page.h"

using namespace NetworkManager;

// 递归布局内容
// 针对于KiranFrame设置不绘制边框
void disableAllFrameDrawBorder(QLayout* layout)
{
    for (int i = 0; i < layout->count(); i++)
    {
        auto item = layout->itemAt(i);
        if (item->layout())
        {
            disableAllFrameDrawBorder(item->layout());
            return;
        }

        auto widget = item->widget();
        if (!widget)
        {
            continue;
        }

        auto frame = qobject_cast<KiranFrame*>(widget);
        if (!frame)
        {
            continue;
        }

        frame->setDrawBroder(false);
    }
}

// 递归布局内容
// 针对于不定长内容的标签（KiranLabel）加入最小宽度设置，并设置省略模式
void setAllKiranLabelElideMode(QLayout* layout)
{
    for (int i = 0; i < layout->count(); i++)
    {
        auto item = layout->itemAt(i);
        if (item->layout())
        {
            setAllKiranLabelElideMode(item->layout());
            return;
        }

        auto widget = item->widget();
        if (widget)
        {
            auto label = qobject_cast<KiranLabel*>(widget);
            if (label)
            {
                label->setMinimumWidth(100);
                label->setElideMode(Qt::ElideRight);
            }
        }
    }
}

namespace Kiran
{
namespace Network
{
enum ActiveConnDataRole
{
    ACTIVE_CONN_ROLE_UUID = Qt::UserRole + 1,
    ACTIVE_CONN_ROLE_TYPE,
    ACTIVE_CONN_ROLE_PATH
};
DetailsPage::DetailsPage(QWidget* parent)
    : Page(PAGE_DETAILS, parent),
      ui(new ::Ui::DetailsPage)
{
    ui->setupUi(this);
    initUI();
    init();
}

DetailsPage::~DetailsPage()
{
    delete ui;
}

void DetailsPage::reset()
{
    if (ui->comboBox->count() >= 2)
    {
        ui->comboBox->setCurrentIndex(0);
    }
    ui->scrollArea->verticalScrollBar()->setValue(0);
}

void DetailsPage::initUI()
{
    // 不显示ipv6网关
    ui->widget_ipv6_gateway->setVisible(false);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    disableAllFrameDrawBorder(ui->layout_detailsContent);
    setAllKiranLabelElideMode(ui->layout_detailsContent);
}

void DetailsPage::init()
{
    // 添加无线+有线的活跃连接
    auto wiredConnections = CM_INSTANCE->activedConncetions(ConnectionType::Wired);
    auto wirelessConnections = CM_INSTANCE->activedConncetions(ConnectionType::Wireless);
    auto activeConnctions = wiredConnections + wirelessConnections;
    for (auto& connectionPath : activeConnctions)
    {
        auto connection = findActiveConnection(connectionPath);
        if (!connection)
        {
            continue;
        }

        auto type = connection->type();
        auto uuid = connection->uuid();
        processActiveConnectionAdded(type, connectionPath, uuid);
    }

    connect(CM_INSTANCE, &ConnectionManager::activeConnectionAdded, this, &DetailsPage::processActiveConnectionAdded);
    connect(CM_INSTANCE, &ConnectionManager::activeConnectionRemoved, this, &DetailsPage::processActiveConnectionRemoved);
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DetailsPage::processCurrentActiveConnChanged);
}

void DetailsPage::resetUI()
{
    QList<QLabel*> labels = {ui->label_security_type,
                             ui->label_frequency_band,
                             ui->label_channel,
                             ui->label_interface,
                             ui->label_mac,
                             ui->label_ipv4_address,
                             ui->label_ipv4_gateway,
                             ui->label_ipv4_dns,
                             ui->label_subnet_mask,
                             ui->label_ipv6_address,
                             ui->label_ipv6_gateway,
                             ui->label_ipv6_dns,
                             ui->label_prefix,
                             ui->label_rate};
    for (auto label : labels)
    {
        label->setText("-");
        label->setStyleSheet("color:#919191;font-family: \"Noto Sans CJK SC Light\";");
    }
}

void DetailsPage::updateConnectionDeviceInfo()
{
    const auto devices = m_activeConnection->devices();
    if (devices.isEmpty())
    {
        return;
    }

    // 活跃连接，目前关联设备只取第一个
    const auto devicePath = devices.at(0);
    const auto device = findNetworkInterface(devicePath);
    if (!device)
    {
        return;
    }
    ui->label_interface->setText(device->interfaceName());

    int bitRate = -1;
    if (device->type() == NetworkManager::Device::Wifi)
    {
        const auto wifiDevice = qSharedPointerDynamicCast<WirelessDevice>(device);

        const auto hardware = wifiDevice->hardwareAddress();
        ui->label_mac->setText(hardware);

        auto ap = wifiDevice->activeAccessPoint();
        if (ap)
        {
            bitRate = ap->maxBitRate();

            const auto frequency = ap->frequency();

            const auto band = NetworkManager::findFrequencyBand(frequency);
            QMap<WirelessSetting::FrequencyBand, QString> bandDescMap = {
                {WirelessSetting::FrequencyBand::A, "5G"},
                {WirelessSetting::FrequencyBand::Bg, "2.4G"},
                {WirelessSetting::FrequencyBand::Automatic, "Automatic"}};
            const auto bandDesc = bandDescMap.value(band);
            ui->label_frequency_band->setText(bandDesc);

            const auto channel = NetworkManager::findChannel(frequency);
            ui->label_channel->setText(QString::number(channel));
        }
    }
    else if (device->type() == NetworkManager::Device::Ethernet)
    {
        const auto wiredDevice = qSharedPointerDynamicCast<WiredDevice>(device);

        const auto hardware = wiredDevice->hardwareAddress();
        ui->label_mac->setText(hardware);

        bitRate = wiredDevice->bitRate();
    }

    if (bitRate >= 0)
    {
        const auto rate = QString("%1 Mb/s").arg(bitRate / 1000);
        ui->label_rate->setText(rate);
    }
}

// 更新连接安全信息
void DetailsPage::updateConnectionSecurityInfo()
{
    const auto conn = m_activeConnection->connection();
    const auto settings = conn->settings();
    const auto wirelessSecurity = settings->setting(Setting::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();
    const auto security8021x = settings->setting(Setting::Security8021x).dynamicCast<Security8021xSetting>();
    Q_UNUSED(security8021x)

    QString securityDesc = tr("Unknown");
    if (wirelessSecurity)
    {
        const auto authalg = wirelessSecurity->authAlg();
        const auto keyMgmt = wirelessSecurity->keyMgmt();
        Q_UNUSED(authalg)
        switch (keyMgmt)
        {
        case WirelessSecuritySetting::KeyMgmt::Unknown:
        case WirelessSecuritySetting::KeyMgmt::Ieee8021x:
        case WirelessSecuritySetting::KeyMgmt::WpaEapSuiteB192:
            securityDesc = tr("Unknown");
            break;
        case WirelessSecuritySetting::KeyMgmt::Wep:
            securityDesc = tr("WEP");
            break;
        case WirelessSecuritySetting::KeyMgmt::WpaNone:
            securityDesc = tr("None");
            break;
        case WirelessSecuritySetting::KeyMgmt::WpaPsk:
            securityDesc = tr("WPA/WPA2 Personal");
            break;
        case WirelessSecuritySetting::KeyMgmt::SAE:
            securityDesc = tr("WPA3 Personal");
            break;
        case WirelessSecuritySetting::KeyMgmt::WpaEap:
            securityDesc = tr("WPA/WPA2 Enterprise");
            break;
        default:
            break;
        }
    }
    else
    {
        securityDesc = "-";
    }
    ui->label_security_type->setText(securityDesc);
}

// 更新IP详细信息
void DetailsPage::updateConnectionIpDetails()
{
    const auto conn = m_activeConnection->connection();
    const auto connSettings = conn->settings();

    // ipv4
    const auto ipv4Config = m_activeConnection->ipV4Config();
    const auto ipv4Gateway = ipv4Config.gateway();
    const auto ipv4Addresses = ipv4Config.addresses();

    QStringList ipv4IpList, ipv4MaskList;
    for (auto& address : ipv4Addresses)
    {
        ipv4IpList << address.ip().toString();
        ipv4MaskList << address.netmask().toString();
    }

    const auto ipv4Setting = connSettings ? connSettings->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>() : nullptr;
    const auto ipv4SettingDNSList = ipv4Setting ? ipv4Setting->dns() : QList<QHostAddress>();
    QStringList manualIpv4DNSList;
    for (auto& dns : ipv4SettingDNSList)
    {
        manualIpv4DNSList << dns.toString();
    }

    const auto dhcp4Config = m_activeConnection->dhcp4Config();
    QStringList dhcp4DNSList;
    if (dhcp4Config)
    {
        auto dhcpOptions = dhcp4Config->options();
        if (dhcpOptions.contains("domain_name_servers"))
        {
            auto temp = dhcpOptions.value("domain_name_servers").toString();
            dhcp4DNSList = temp.split(" ");
        }
    }

    // 自动获取IP（DNS）后，网络详情显示自动获取到的DNS
    // 手动配置DNS，网络详情中DNS服务器仅显示手动配置的DNS
    auto temp4DNSList = manualIpv4DNSList.isEmpty() ? dhcp4DNSList : manualIpv4DNSList;
    auto dns4Detail = temp4DNSList.size() ? temp4DNSList.join(";") : "-";

    ui->label_ipv4_address->setText(ipv4IpList.join(";"));
    ui->label_ipv4_address->setToolTip(ipv4IpList.join(";"));
    ui->label_subnet_mask->setText(ipv4MaskList.join(";"));
    ui->label_subnet_mask->setToolTip(ipv4MaskList.join(";"));
    ui->label_ipv4_gateway->setText(ipv4Gateway);
    ui->label_ipv4_dns->setText(dns4Detail);

    // ipv6
    const auto ipv6Config = m_activeConnection->ipV6Config();
    const auto ipv6Gateway = ipv6Config.gateway();
    const auto ipv6Addresses = ipv6Config.addresses();

    QStringList ipv6IPList, ipv6PrefixList;
    for (auto& address : ipv6Addresses)
    {
        ipv6IPList << address.ip().toString();
        ipv6PrefixList << QString::number(address.prefixLength());
    }

    const auto ipv6Setting = connSettings ? connSettings->setting(Setting::Ipv6).dynamicCast<Ipv6Setting>() : nullptr;
    const auto ipv6SettingDNSList = ipv6Setting ? ipv6Setting->dns() : QList<QHostAddress>();
    QStringList manualIpv6DNSList;
    for (auto& dns : ipv6SettingDNSList)
    {
        manualIpv6DNSList << dns.toString();
    }

    const auto dhcp6Config = m_activeConnection->dhcp6Config();
    QStringList dhcp6DNSList;
    if (dhcp6Config)
    {
        auto dhcpOptions = dhcp6Config->options();
        if (dhcpOptions.contains("domain_name_servers"))
        {
            auto temp = dhcpOptions.value("domain_name_servers").toString();
            dhcp6DNSList = temp.split(" ");
        }
    }

    auto temp6DNSList = manualIpv6DNSList.isEmpty() ? dhcp6DNSList : manualIpv6DNSList;
    auto dns6Detail = temp6DNSList.size() ? temp6DNSList.join(";") : "-";

    ui->label_ipv6_address->setText(ipv6IPList.join(";"));
    ui->label_ipv6_address->setToolTip(ipv6IPList.join(";"));
    ui->label_prefix->setText(ipv6PrefixList.join(";"));
    ui->label_prefix->setToolTip(ipv6PrefixList.join(";"));
    ui->label_ipv6_gateway->setText(ipv6Gateway);
    ui->label_ipv6_dns->setText(dns6Detail);
}

void DetailsPage::processActiveConnectionAdded(ConnectionType type, const QString& path, const QString& uuid)
{
    auto connection = findActiveConnection(path);
    if (!connection)
        return;

    // 新增活跃连接项
    auto connName = connection->id();
    ui->comboBox->addItem(connName);

    int itemIdx = ui->comboBox->count() - 1;
    ui->comboBox->setItemData(itemIdx, path, ACTIVE_CONN_ROLE_PATH);
    ui->comboBox->setItemData(itemIdx, uuid, ACTIVE_CONN_ROLE_UUID);
    ui->comboBox->setItemData(itemIdx, type, ACTIVE_CONN_ROLE_TYPE);

    KLOG_DEBUG(qLcNetwork) << "active connection added:" << connName << path;

    // 检查当前项是否为活跃项，处理从无至有，触发当前项变化
    if (itemIdx == ui->comboBox->currentIndex())
    {
        processCurrentActiveConnChanged();
    }

    // NOTE: 单个活跃连接也不去除活跃连接下拉框，提供活跃连接配置名展示
    //  ui->widget_mulitConntion->setVisible((ui->comboBox->count() >= 2));
}

void DetailsPage::processActiveConnectionRemoved(ConnectionType type, const QString& path, const QString& uuid)
{
    auto index = ui->comboBox->findData(path, ACTIVE_CONN_ROLE_PATH);
    if (index != -1)
    {
        auto connName = ui->comboBox->itemText(index);
        if (ui->comboBox->currentIndex() == index)
        {
            processCurrentActiveConnChanged();
        }
        KLOG_DEBUG(qLcNetwork) << "active connection removed:" << connName;
        ui->comboBox->removeItem(index);
    }

    // NOTE: 单个活跃连接也不去除活跃连接下拉框，提供活跃连接配置名展示
    //  ui->widget_mulitConntion->setVisible((ui->comboBox->count() >= 2));
}

void DetailsPage::processCurrentActiveConnChanged()
{
    if (m_activeConnection)
    {
        disconnect(m_activeConnection.data(), &ActiveConnection::connectionChanged, this, &DetailsPage::loadActiveConnectionInfo);
        disconnect(m_activeConnection.data(), &ActiveConnection::dhcp4ConfigChanged, this, &DetailsPage::loadActiveConnectionInfo);
        disconnect(m_activeConnection.data(), &ActiveConnection::ipV4ConfigChanged, this, &DetailsPage::loadActiveConnectionInfo);
        disconnect(m_activeConnection.data(), &ActiveConnection::ipV6ConfigChanged, this, &DetailsPage::loadActiveConnectionInfo);
        m_activeConnection.clear();
    }

    auto pathVar = ui->comboBox->currentData(ACTIVE_CONN_ROLE_PATH);
    auto activeConn = findActiveConnection(pathVar.toString());

    // 未找到活跃连接信息
    if (!activeConn)
    {
        KLOG_DEBUG(qLcNetwork) << "current active connection is null"
                               << "path:" << pathVar.toString()
                               << "name" << ui->comboBox->currentText();
        resetUI();
        return;
    }

    // 读取当前活跃信息
    m_activeConnection = activeConn;
    KLOG_DEBUG(qLcNetwork) << "current active connection detail:" << m_activeConnection->id();

    loadActiveConnectionInfo();
    connect(m_activeConnection.data(), &ActiveConnection::connectionChanged, this, &DetailsPage::loadActiveConnectionInfo);
    connect(m_activeConnection.data(), &ActiveConnection::dhcp4ConfigChanged, this, &DetailsPage::loadActiveConnectionInfo);
    connect(m_activeConnection.data(), &ActiveConnection::ipV4ConfigChanged, this, &DetailsPage::loadActiveConnectionInfo);
    connect(m_activeConnection.data(), &ActiveConnection::ipV6ConfigChanged, this, &DetailsPage::loadActiveConnectionInfo);
}

// 更新当前选中激活连接信息
void DetailsPage::loadActiveConnectionInfo()
{
    resetUI();

    const auto connection = m_activeConnection->connection();
    const auto connectionSettings = connection->settings();
    if (!connectionSettings)
    {
        KLOG_ERROR(qLcNetwork) << "load active connection info failed, connection settings is null:"
                               << m_activeConnection->id();
        return;
    }

    updateConnectionDeviceInfo();

    const auto connectionType = m_activeConnection->type();
    const auto isWireless = connectionType == ConnectionType::Wireless;
    ui->widget_wireless->setVisible(isWireless);
    if (isWireless)
    {
        updateConnectionSecurityInfo();
    }

    updateConnectionIpDetails();
}

}  // namespace Network
}  // namespace Kiran