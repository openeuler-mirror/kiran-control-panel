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

#include "connection-name-widget.h"
#include <kiran-message-box.h>
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessSetting>
#include "ui_connection-name-widget.h"
using namespace NetworkManager;

ConnectionNameWidget::ConnectionNameWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ConnectionNameWidget)
{
    ui->setupUi(this);
    initUI();
}

ConnectionNameWidget::~ConnectionNameWidget()
{
    delete ui;
}

void ConnectionNameWidget::initUI()
{
    m_autoConnection = new KiranSwitchButton(this);
    m_autoConnection->setAccessibleName(QString("SwitchAutoConnection"));
    ui->autoConnectionLayout->addWidget(m_autoConnection);
    m_autoConnection->setChecked(true);
    ui->connectionName->setPlaceholderText(tr("Required"));
}

void ConnectionNameWidget::setConnectionSettings(const ConnectionSettings::Ptr &connectionSettings)
{
    m_connectionSettings = connectionSettings;
}

void ConnectionNameWidget::setNameLabel(const QString &name)
{
    ui->nameLabel->setText(name);
}

void ConnectionNameWidget::saveSettings()
{
    if (m_connectionSettings != nullptr)
    {
        QString connectionId = ui->connectionName->text();
        m_connectionSettings->setId(connectionId);
        m_connectionSettings->setAutoconnect(m_autoConnection->isChecked());
    }
}

void ConnectionNameWidget::showSettings(ConnectionSettings::ConnectionType connectionType)
{
    m_connectionType = connectionType;
    if (m_connectionType == ConnectionSettings::Wired)
    {
        if (m_connectionSettings != nullptr)
        {
            QString connectionId = m_connectionSettings->id();
            ui->connectionName->setText(connectionId);
            m_autoConnection->setChecked(m_connectionSettings->autoconnect());
        }
        else
        {
            QString connectionName = tr("Wired Connection %1");
            //生成名称数字后缀
            QString connectionNameStr = connectionName.arg(connectionSuffixNum(connectionName));
            ui->connectionName->setText(connectionNameStr);
            m_autoConnection->setChecked(false);
        }
    }
    else if (m_connectionType == ConnectionSettings::Wireless)
    {
        if (m_connectionSettings != nullptr)
        {
            WirelessSetting::Ptr wirelessSetting = m_connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
            ui->connectionName->setText(wirelessSetting->ssid());
            ui->connectionName->setEnabled(false);
            m_autoConnection->setChecked(m_connectionSettings->autoconnect());
        }
        else
        {
            //            ui->connectionName->setEnabled(true);
            //            m_autoConnection->setChecked(true);
        }
    }
}

void ConnectionNameWidget::showVpnSettings(VpnType vpnType)
{
    m_connectionType = ConnectionSettings::ConnectionType::Vpn;
    if (m_connectionSettings != nullptr)
    {
        QString connectionId = m_connectionSettings->id();
        ui->connectionName->setText(connectionId);
        m_autoConnection->setChecked(m_connectionSettings->autoconnect());
    }
    else
    {
        QString connectionName = "";
        switch (vpnType)
        {
        case VpnType::VPN_TYPE_L2TP:
            connectionName = tr("VPN L2TP %1");
            break;
        case VpnType::VPN_TYPE_PPTP:
            connectionName = tr("VPN PPTP %1");
            break;
        default:
            break;
        }

        if (!connectionName.isEmpty())
        {
            //生成名称数字后缀
            QString connectionNameStr = connectionName.arg(connectionSuffixNum(connectionName));
            ui->connectionName->setText(connectionNameStr);
        }
        m_autoConnection->setChecked(false);
    }
}

int ConnectionNameWidget::connectionSuffixNum(QString &connName)
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
        if (conn->settings()->connectionType() == m_connectionType)
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

void ConnectionNameWidget::clearPtr()
{
    m_connectionSettings.clear();
}

bool ConnectionNameWidget::isInputValid()
{
    QString nameStr = ui->connectionName->text();
    if (nameStr.isEmpty())
    {
        QString error = QString(tr("Connection name can not be empty"));
        KiranMessageBox::KiranStandardButton btn = KiranMessageBox::message(this, tr("Error"),
                                                                            error,
                                                                            KiranMessageBox::Yes | KiranMessageBox::No);

        KLOG_DEBUG() << "Connection name cannot be empty";
        return false;
    }
    return true;
}