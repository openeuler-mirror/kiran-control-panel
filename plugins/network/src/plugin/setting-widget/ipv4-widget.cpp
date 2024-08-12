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
    ui->ipv4Address->setPlaceholderText(tr("Required,separated multiple entries by semicolon"));
    ui->ipv4Netmask->setPlaceholderText(tr("Required,separate multiple entries by semicolon"));
    ui->ipv4DNS->setPlaceholderText(tr("Please separate multiple DNS entries by semicolon"));
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

    Ipv4Setting::ConfigMethod method = ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>();
    if (method == Ipv4Setting::ConfigMethod::Automatic)
    {
        IpAddress emptyAddress;
        emptyAddress.setIp(QHostAddress(""));
        emptyAddress.setNetmask(QHostAddress(""));
        emptyAddress.setGateway(QHostAddress(""));

        m_ipv4Setting->setMethod(method);
        m_ipv4Setting->setAddresses(QList<NetworkManager::IpAddress>() << emptyAddress);
    }
    else if (method == Ipv4Setting::ConfigMethod::Manual)
    {
        m_ipv4Setting->setMethod(method);

        QList<IpAddress> ipv4AddresseList;
        auto ipVec = ui->ipv4Address->text().split(';').toVector();
        auto maskVec = ui->ipv4Netmask->text().split(';').toVector();
        auto gateway = ui->ipv4Gateway->text();
        for ( int i=0; i<ipVec.size();i++ )
        {
            IpAddress ipv4Address;
            QString ipStr = ipVec[i];

            ipv4Address.setIp(QHostAddress(ipStr));

            if( i >= maskVec.size() )
            {
                KLOG_WARNING() << ipStr << "lack net prefix.";
                continue;
            }
            else
            {
                QString netMaskTemp = maskVec.at(i);
                if( !netMaskTemp.contains(".") ) //十进制掩码
                {
                    int netPrefix = netMaskTemp.toInt();
                    if ( netPrefix <= 0 || netPrefix >= 33 )
                    {
                        KLOG_WARNING() << ipStr << "net prefix length error";
                        continue;
                    }
                    ipv4Address.setPrefixLength(netPrefix);
                }
                else //点分十进制
                {
                    ipv4Address.setNetmask(QHostAddress(netMaskTemp));
                }
            }
            ipv4Address.setGateway(QHostAddress(gateway));
            KLOG_DEBUG() << "ipv4 name:" << m_ipv4Setting->name();
            KLOG_DEBUG() << "\tappend" << ipv4Address.ip().toString() 
                                   << ipv4Address.netmask().toString() 
                                   << ipv4Address.gateway().toString()
                                   << ipv4Address.gateway().toString();

            ipv4AddresseList << ipv4Address;
        }
        m_ipv4Setting->setAddresses(ipv4AddresseList);
    }

    QList<QHostAddress> ipv4DNS;
    if (!ui->ipv4DNS->text().isEmpty())
    {
        //多个DNS以分号分隔
        QString dnsString = ui->ipv4DNS->text();
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
        QStringList dnsList = dnsString.split(";",QString::SkipEmptyParts);
#else
        QStringList dnsList = dnsString.split(";",Qt::SkipEmptyParts);
#endif
        for(auto dns : dnsList)
        {
            ipv4DNS << QHostAddress(dns);
        }
    }
    KLOG_DEBUG() << "ipv4 DNS:" << ipv4DNS;
    m_ipv4Setting->setDns(ipv4DNS);
}

void Ipv4Widget::showSettings()
{
    if(m_ipv4Setting.isNull())
    {
        resetSettings();
        return;
    }

    KLOG_DEBUG() << "current ipv4 Setting method:" << m_ipv4Setting->method();
    if (m_ipv4Setting->method() == Ipv4Setting::ConfigMethod::Automatic)
    {
        resetSettings();
    }
    else if (m_ipv4Setting->method() == Ipv4Setting::ConfigMethod::Manual)
    {
        int ipv4MethodIndex = ui->ipv4Method->findData(m_ipv4Setting->method());
        ui->ipv4Method->setCurrentIndex(ipv4MethodIndex);

        QList<QString> ipList;
        QList<QString> maskList;
        QString gateway;
        auto ipAdresss = m_ipv4Setting->addresses(); 
        for ( auto ipv4Address : ipAdresss )
        {
            QString address = ipv4Address.ip().toString();
            ipList << address;

            QString netmask = ipv4Address.netmask().toString();
            maskList << netmask;

            QString temp = ipv4Address.gateway().toString();
            if(temp != "0.0.0.0")
            {
                gateway = temp;
            }
        }

        ui->ipv4Address->setText(ipList.join(';'));
        ui->ipv4Netmask->setText(maskList.join(';'));
        ui->ipv4Gateway->setText(gateway);
    }

    QString dnsString = "";
    if (!m_ipv4Setting->dns().isEmpty())
    {        
        QStringList dnsList;
        auto hostAddressList = m_ipv4Setting->dns();
        for(auto address: hostAddressList)
        {
            dnsList << address.toString();
        }
        dnsString = dnsList.join(";");
        KLOG_DEBUG() << "ipv4 DNS:" << dnsString;
    }
    ui->ipv4DNS->setText(dnsString);   
}

void Ipv4Widget::resetSettings()
{
    int ipv4MethodIndex = ui->ipv4Method->findData(Ipv4Setting::ConfigMethod::Automatic);
    ui->ipv4Method->setCurrentIndex(ipv4MethodIndex);
    ui->ipv4Address->clear();
    ui->ipv4Netmask->clear();
    ui->ipv4Gateway->clear();
    ui->ipv4DNS->clear();
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
        if(!isIpv4ManualConfigValid())
        {
            return false;
        }
    }

    QString dnsString = ui->ipv4DNS->text();
    if (!dnsString.isEmpty())
    {
        bool valid = true;
        auto dnsList = dnsString.split(";");
        for(auto dns : dnsList)
        {
            if(!isIpv4AddressValid(dns))
            {
                valid = false;
                break;
            }
        }

        if(!valid)
        {
            QString error = QString(tr("Ipv4 DNS invalid"));
            m_errorTip->setText(error);
            m_errorTip->showTipAroundWidget(ui->ipv4DNS);
            KLOG_DEBUG() << "Ipv4 DNS invalid";
            return false;    
        }
    }

    return true;
}

bool Ipv4Widget::isIpv4ManualConfigValid()
{
    int ipCount = 0;
    int maskCount = 0;
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
        auto ipVector = ipv4.split(";").toVector();
        ipCount = ipVector.size();

        for (int i = 0; i < ipCount; i++)
        {
            if (!isIpv4AddressValid(ipVector[i]))
            {
                QString error = QString(tr("Ipv4 address is invalid"));
                m_errorTip->setText(error);
                m_errorTip->showTipAroundWidget(ui->ipv4Address);

                KLOG_DEBUG() << "Ipv4 address is invalid";
                return false;
            }
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
        auto netMaskVector = netMask.split(";").toVector();
        maskCount = netMaskVector.size();
        for (auto netMask : netMaskVector)
        {
            if (!isIpv4NetmaskValid(netMask))
            {
                QString error = QString(tr("NetMask is invalid"));
                m_errorTip->setText(error);
                m_errorTip->showTipAroundWidget(ui->ipv4Netmask);
                KLOG_DEBUG() << "NetMaskis invalid";

                return false;
            }
        }
    }

    if( ipCount != maskCount )
    {
        QString error = QString(tr("The number of IPs and masks cannot correspond"));
        m_errorTip->setText(error);
        m_errorTip->showTipAroundWidget(ui->ipv4Netmask);
        return false;
    }

    if( ipCount > 10 || maskCount > 10 )
    {
        QString error = QString(tr("The entries of IPs and masks cannot exceed 10"));
        m_errorTip->setText(error);
        m_errorTip->showTipAroundWidget(ipCount>10?ui->ipv4Address:ui->ipv4Netmask);
        return false;
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
