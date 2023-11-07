/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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

#include "ipv4-widget.h"
#include <kiran-message-box.h>
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include "kiran-tips/kiran-tips.h"
#include "ui_ipv4-widget.h"
using namespace NetworkManager;

Ipv4Widget::Ipv4Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Ipv4Widget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

Ipv4Widget::~Ipv4Widget()
{
    delete ui;
}

void Ipv4Widget::initUI()
{
    ui->ipv4Method->addItem(tr("Auto"), Ipv4Setting::ConfigMethod::Automatic);
    ui->ipv4Method->addItem(tr("Manual"), Ipv4Setting::ConfigMethod::Manual);
    ui->ipv4Manual->setVisible(false);
    ui->ipv4Address->setPlaceholderText(tr("Required"));
    ui->ipv4Netmask->setPlaceholderText(tr("Required"));
}

void Ipv4Widget::initConnection()
{
    connect(ui->ipv4Method, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
            { handleIpv4MethodChanged(ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>()); });
}

void Ipv4Widget::handleIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method)
{
    switch (method)
    {
    case Ipv4Setting::ConfigMethod::Automatic:
        ui->ipv4Manual->setVisible(false);
        break;
    case Ipv4Setting::ConfigMethod::Manual:
        ui->ipv4Manual->setVisible(true);
        break;
    default:
        break;
    }
}

void Ipv4Widget::setIpv4Setting(const Ipv4Setting::Ptr &ipv4Setting)
{
    m_ipv4Setting = ipv4Setting;
}

void Ipv4Widget::setErrorTips(KiranTips *errorTips)
{
    m_errorTip = errorTips;
}

void Ipv4Widget::saveSettings()
{
    if (m_ipv4Setting == nullptr)
    {
        return;
    }

    IpAddress ipv4Address;
    Ipv4Setting::ConfigMethod method = ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>();
    if (method == Ipv4Setting::ConfigMethod::Automatic)
    {
        m_ipv4Setting->setMethod(method);
        ipv4Address.setIp(QHostAddress(""));
        ipv4Address.setNetmask(QHostAddress(""));
        ipv4Address.setGateway(QHostAddress(""));
        m_ipv4Setting->setAddresses(QList<NetworkManager::IpAddress>() << ipv4Address);
    }
    else if (method == Ipv4Setting::ConfigMethod::Manual)
    {
        m_ipv4Setting->setMethod(method);

        ipv4Address.setIp(QHostAddress(ui->ipv4Address->text()));
        QString netMask = ui->ipv4Netmask->text();
        if (!netMask.contains("."))
        {
            int netPrefix = netMask.toInt();
            KLOG_DEBUG() << "netMask.toInt():" << netMask.toInt();
            if ((netPrefix > 0) & (netPrefix < 33))
            {
                ipv4Address.setPrefixLength(netPrefix);
            }
            else
            {
                KLOG_DEBUG() << "Net prefix length error";
            }
        }
        else
        {
            ipv4Address.setNetmask(QHostAddress(netMask));
        }

        ipv4Address.setGateway(QHostAddress(ui->ipv4Gateway->text()));
        KLOG_DEBUG() << "ipv4Address.ip():" << ipv4Address.ip();
        KLOG_DEBUG() << "ipv4Address.netmask():" << ipv4Address.netmask();
        KLOG_DEBUG() << "ipv4Address.prefixLength():" << ipv4Address.prefixLength();
        KLOG_DEBUG() << "ipv4Address.gateway():" << ipv4Address.gateway();

        QList<IpAddress> ipv4AddresseList;
        ipv4AddresseList << ipv4Address;
        m_ipv4Setting->setAddresses(ipv4AddresseList);
    }

    QList<QHostAddress> ipv4DNS;
    if (!ui->ipv4FirstDNS->text().isEmpty())
    {
        ipv4DNS << QHostAddress(ui->ipv4FirstDNS->text());
    }
    if (!ui->ipv4SecondDNS->text().isEmpty())
    {
        ipv4DNS << QHostAddress(ui->ipv4SecondDNS->text());
    }
    KLOG_DEBUG() << "ipv4DNS:" << ipv4DNS;
    m_ipv4Setting->setDns(ipv4DNS);
    
}

void Ipv4Widget::showSettings()
{
    if (m_ipv4Setting != nullptr)
    {
        KLOG_DEBUG() << "m_ipv4Setting->method():" << m_ipv4Setting->method();

        if (m_ipv4Setting->method() == Ipv4Setting::ConfigMethod::Automatic)
        {
            KLOG_DEBUG() << "Ipv4Setting::ConfigMethod::Automatic";
            resetSettings();
        }
        else if (m_ipv4Setting->method() == Ipv4Setting::ConfigMethod::Manual)
        {
            int ipv4MethodIndex = ui->ipv4Method->findData(m_ipv4Setting->method());
            ui->ipv4Method->setCurrentIndex(ipv4MethodIndex);
            // xxx:取addresses的方式有待改进
            IpAddress ipv4Address = m_ipv4Setting->addresses().value(0);
            QString address = ipv4Address.ip().toString();
            QString netmask = ipv4Address.netmask().toString();
            QString gateway = ipv4Address.gateway().toString();

            KLOG_DEBUG() << "address:" << address;
            KLOG_DEBUG() << "netmask:" << netmask;
            KLOG_DEBUG() << "gateway:" << gateway;

            ui->ipv4Address->setText(address);
            ui->ipv4Netmask->setText(netmask);
            if(gateway != "0.0.0.0")
            {
                ui->ipv4Gateway->setText(gateway);
            }
            else
            {
                ui->ipv4Gateway->clear();
            }
        }
        QString firstDNS = "";
        QString secondDNS = "";
        if (!m_ipv4Setting->dns().isEmpty())
        {
            firstDNS = m_ipv4Setting->dns().at(0).toString();
            if (m_ipv4Setting->dns().count() >= 2)
            {
                secondDNS = m_ipv4Setting->dns().at(1).toString();
            }
        }
        KLOG_DEBUG() << "firstDNS:" << firstDNS;
        KLOG_DEBUG() << "secondDNS:" << secondDNS;
        ui->ipv4FirstDNS->setText(firstDNS);
        ui->ipv4SecondDNS->setText(secondDNS);
    }
    else
        resetSettings();
}

void Ipv4Widget::resetSettings()
{
    int ipv4MethodIndex = ui->ipv4Method->findData(Ipv4Setting::ConfigMethod::Automatic);
    ui->ipv4Method->setCurrentIndex(ipv4MethodIndex);
    ui->ipv4Address->clear();
    ui->ipv4Netmask->clear();
    ui->ipv4Gateway->clear();
    ui->ipv4FirstDNS->clear();
    ui->ipv4SecondDNS->clear();
}

void Ipv4Widget::clearPtr()
{
    m_ipv4Setting.clear();
}

bool Ipv4Widget::isInputValid()
{
    Ipv4Setting::ConfigMethod configMethod = ui->ipv4Method->currentData().value<Ipv4Setting::ConfigMethod>();

    if (configMethod == Ipv4Setting::ConfigMethod::Automatic)
    {
    }
    else if (configMethod == Ipv4Setting::ConfigMethod::Manual)
    {
        QString ipv4 = ui->ipv4Address->text();
        QString netMask = ui->ipv4Netmask->text();
        QString ipv4Gateway = ui->ipv4Gateway->text();

        if (ipv4.isEmpty())
        {
            QString error = QString(tr("Ipv4 address can not be empty"));
            m_errorTip->setText(error);
            m_errorTip->showTipAroundWidget(ui->ipv4Address);
            KLOG_DEBUG() << "Ipv4 address can not be empty";
            return false;
        }
        else
        {
            if (!isIpv4AddressValid(ipv4))
            {
                QString error = QString(tr("Ipv4 Address invalid"));
                m_errorTip->setText(error);
                m_errorTip->showTipAroundWidget(ui->ipv4Address);
                KLOG_DEBUG() << "Ipv4 Address invalid";
                return false;
            }
        }

        if (netMask.isEmpty())
        {
            QString error = QString(tr("NetMask can not be empty"));
            m_errorTip->setText(error);
            m_errorTip->showTipAroundWidget(ui->ipv4Netmask);
            KLOG_DEBUG() << "NetMask cannot be empty";
            return false;
        }
        else
        {
            if (!isIpv4NetmaskValid(netMask))
            {
                QString error = QString(tr("Netmask invalid"));
                m_errorTip->setText(error);
                m_errorTip->showTipAroundWidget(ui->ipv4Netmask);
                KLOG_DEBUG() << "Netmask invalid";
                return false;
            }
        }

        if (!ipv4Gateway.isEmpty())
        {
            if (!isIpv4AddressValid(ipv4Gateway))
            {
                QString error = QString(tr("Ipv4 Gateway invalid"));
                m_errorTip->setText(error);
                m_errorTip->showTipAroundWidget(ui->ipv4Gateway);
                return false;
            }
        }
    }

    QString firstDNS = ui->ipv4FirstDNS->text();
    if (!firstDNS.isEmpty())
    {
        if (!isIpv4AddressValid(firstDNS))
        {
            QString error = QString(tr("Ipv4 Preferred DNS invalid"));
            m_errorTip->setText(error);
            m_errorTip->showTipAroundWidget(ui->ipv4FirstDNS);
            KLOG_DEBUG() << "Ipv4 Preferred DNS invalid";
            return false;
        }
    }

    QString secondDNS = ui->ipv4SecondDNS->text();
    if (!secondDNS.isEmpty())
    {
        if (!isIpv4AddressValid(secondDNS))
        {
            QString error = QString(tr("Ipv4 Alternate DNS invalid"));
            m_errorTip->setText(error);
            m_errorTip->showTipAroundWidget(ui->ipv4SecondDNS);
            KLOG_DEBUG() << "Ipv4 Alternate DNS invalid";
            return false;
        }
    }

    return true;
}

bool Ipv4Widget::isIpv4AddressValid(const QString &address)
{
    QHostAddress ipAddr(address);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv4) || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
    {
        return false;
    }
    QRegExp regExpIP("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
    return regExpIP.exactMatch(address);
}

bool Ipv4Widget::isIpv4NetmaskValid(const QString &address)
{
    bool done;
    quint32 mask = QHostAddress(address).toIPv4Address(&done);

    if (!done)
    {
        return false;
    }

    for (; mask != 0; mask <<= 1)
    {
        if ((mask & (static_cast<uint>(1) << 31)) == 0)
            return false;  // Highest bit is now zero, but mask is non-zero.
    }
    QRegExp regExpIP("^((128|192)|2(24|4[08]|5[245]))(\\.(0|(128|192)|2((24)|(4[08])|(5[245])))){3}$");
    return regExpIP.exactMatch(address);
}
