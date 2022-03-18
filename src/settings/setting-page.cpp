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

#include "setting-page.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/AdslSetting>
#include <NetworkManagerQt/Settings>

SettingPage::SettingPage(QWidget* parent) : QWidget(parent)
{
}

SettingPage::~SettingPage()
{
}

void SettingPage::initConnectionSettings(ConnectionSettings::ConnectionType connectionType, const QString& connectionUuid)
{
    m_connectionType = connectionType;
    m_connectionUuid = connectionUuid;

    if (m_connectionUuid.isEmpty())
    {
        KLOG_DEBUG() << "connection uuid is empty, creating new ConnectionSettings";
        createConnectionSettings();
        m_isNewConnection = true;
    }
    else
    {
        m_connection = findConnectionByUuid(m_connectionUuid);
        if (!m_connection)
        {
            KLOG_DEBUG() << "can't find connection by uuid";
        }
        m_connectionSettings = m_connection->settings();

        m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
        m_ipv6Setting = m_connectionSettings->setting(Setting::SettingType::Ipv6).dynamicCast<Ipv6Setting>();

        m_isNewConnection = false;
    }
}

void SettingPage::createConnectionSettings()
{
    m_connectionSettings = QSharedPointer<ConnectionSettings>(new NetworkManager::ConnectionSettings(m_connectionType));
    m_connectionUuid = m_connectionSettings->createNewUuid();
    m_connectionSettings->setUuid(m_connectionUuid);
    KLOG_DEBUG() << "create uuid:" << m_connectionSettings->uuid();

    m_ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
    m_ipv6Setting = m_connectionSettings->setting(Setting::SettingType::Ipv6).dynamicCast<Ipv6Setting>();
}

void SettingPage::clearPtr()
{
    m_connection.clear();
    m_connectionSettings.clear();
    m_ipv4Setting.clear();
    m_ipv6Setting.clear();
}

int SettingPage::connectionSuffixNum(QString& connName, ConnectionSettings::ConnectionType connType)
{
    if (connName.isEmpty())
    {
        return 0;
    }

    NetworkManager::Connection::List connList = listConnections();
    QStringList connNameList;
    int connSuffixNum = 1;

    for (auto conn : connList)
    {
        if (conn->settings()->connectionType() == connType)
        {
            connNameList.append(conn->name());
        }
    }

    for (int i = 1; i <= connNameList.size(); ++i)
    {
        if (!connNameList.contains(connName.arg(i)))
        {
            connSuffixNum = i;
            break;
        }
        else if (i == connNameList.size())
        {
            connSuffixNum = i + 1;
        }
    }

    return connSuffixNum;
}

void SettingPage::handleDeleteConnection()
{
    QString tip = QString(tr("Are you sure you want to delete the connection %1")).arg(m_connection->name());
    KiranMessageBox::KiranStandardButton btn = KiranMessageBox::message(this, tr("Warning"),
                                                                        tip,
                                                                        KiranMessageBox::Yes | KiranMessageBox::No);
    if (btn == KiranMessageBox::Yes)
    {
        m_connection->remove();
        //暂不需要添加删除的信号，删除成功后dbus和NetworkManager库会返回相关信号
        emit settingChanged();
    }
}

void SettingPage::setIpv4Settings(CommConfigInfo& configInfo)
{
    if (configInfo.ipv4Method == Ipv4Setting::ConfigMethod::Automatic)
    {
        configInfo.ipv4Address = "";
        configInfo.ipv4Netmask = "";
        configInfo.ipv4Gateway = "";
    }
    else
    {
    }

    m_ipv4Setting->setMethod(configInfo.ipv4Method);

    IpAddress ipv4Address;
    ipv4Address.setIp(QHostAddress(configInfo.ipv4Address));
    ipv4Address.setNetmask(QHostAddress(configInfo.ipv4Netmask));
    ipv4Address.setGateway(QHostAddress(configInfo.ipv4Gateway));

    QList<IpAddress> ipv4AddresseList;
    ipv4AddresseList << ipv4Address;
    m_ipv4Setting->setAddresses(ipv4AddresseList);

    QList<QHostAddress> ipv4DNS;
    ipv4DNS << QHostAddress(configInfo.ipv4PreferredDNS) << QHostAddress(configInfo.ipv4AlternateDNS);
    m_ipv4Setting->setDns(ipv4DNS);
}

void SettingPage::setWiredSettings(CommConfigInfo& configInfo)
{
    QString macAddress = configInfo.ethernetDeviceMac;
    QString cloneMac = configInfo.ethernetCloneDeviceMac;
    KLOG_DEBUG() << "macAddress:" << macAddress;
    KLOG_DEBUG() << "cloneMac:" << cloneMac;
    m_wiredSetting->setMacAddress(QByteArray::fromHex(macAddress.toUtf8()));
    m_wiredSetting->setClonedMacAddress(QByteArray::fromHex(cloneMac.toUtf8()));
    m_wiredSetting->setMtu(configInfo.mtu);
}
