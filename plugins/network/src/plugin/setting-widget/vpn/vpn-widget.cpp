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

#include "vpn-widget.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <QHostAddress>
#include "kiran-tips/kiran-tips.h"
#include "logging-category.h"
#include "ui_vpn-widget.h"
using namespace NetworkManager;

VpnWidget::VpnWidget(QWidget *parent) : QWidget(parent), ui(new Ui::VpnWidget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
    ui->gateway->setPlaceholderText(tr("Required"));
    ui->userName->setPlaceholderText(tr("Required"));
    ui->password->setPlaceholderText(tr("Required"));
}

VpnWidget::~VpnWidget()
{
    delete ui;
}

void VpnWidget::initUI()
{
    ui->passwordOptions->addItem(tr("Saved"), Setting::SecretFlagType::None);
    ui->passwordOptions->addItem(tr("Ask"), Setting::SecretFlagType::NotSaved);
    ui->passwordOptions->addItem(tr("Not required"), Setting::SecretFlagType::NotRequired);

    ui->passwordOptions->setFocusPolicy(Qt::NoFocus);
    ui->password->setEchoMode(QLineEdit::Password);
    ui->passwordVisual->setVisible(true);
}

void VpnWidget::initConnection()
{
    connect(ui->passwordOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index)
            { handlePasswordOptionsChanged(ui->passwordOptions->currentData().value<Setting::SecretFlagType>()); });
    connect(ui->passwordVisual, &QPushButton::clicked, this, &VpnWidget::enablePasswordVisual);
}
void VpnWidget::setVpnSetting(const VpnSetting::Ptr &vpnSetting)
{
    m_vpnSetting = vpnSetting;
}

void VpnWidget::setConnectionPtr(const Connection::Ptr &connection)
{
    m_connection = connection;
}

void VpnWidget::setErrorTips(KiranTips *errorTips)
{
    m_errorTip = errorTips;
}

void VpnWidget::handlePasswordOptionsChanged(Setting::SecretFlagType secretFlagType)
{
    switch (secretFlagType)
    {
    case Setting::SecretFlagType::None:
        ui->passwordWidget->setVisible(true);
        break;
    case Setting::SecretFlagType::AgentOwned:
        ui->passwordWidget->setVisible(true);
        break;
    case Setting::SecretFlagType::NotSaved:
        ui->passwordWidget->setVisible(false);
        break;
    case Setting::SecretFlagType::NotRequired:
        ui->passwordWidget->setVisible(false);
        break;
    }
}

void VpnWidget::enablePasswordVisual()
{
    if (ui->password->echoMode() == QLineEdit::Password)
    {
        ui->password->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->password->setEchoMode(QLineEdit::Password);
    }
}

void VpnWidget::saveSettings()
{
    if (m_vpnSetting != nullptr)
    {
        m_dataMap = m_vpnSetting->data();
        m_secretMap = m_vpnSetting->secrets();

        m_dataMap.insert("gateway", ui->gateway->text());
        m_dataMap.insert("user", ui->userName->text());
        Setting::SecretFlagType secretFlag = ui->passwordOptions->currentData().value<Setting::SecretFlagType>();
        m_dataMap.insert("password-flags", QString::number(secretFlag));

        if (secretFlag == Setting::SecretFlagType::None)
            m_secretMap.insert("password", ui->password->text());
        else
            m_secretMap.remove("password");

        if (!ui->ntDomain->text().isEmpty())
        {
            m_dataMap.insert("domain", ui->ntDomain->text());
        }

        m_vpnSetting->setData(m_dataMap);
        m_vpnSetting->setSecrets(m_secretMap);
        m_vpnSetting->setInitialized(true);
    }
}

void VpnWidget::showSettings()
{
    if (m_vpnSetting != nullptr)
    {
        NMStringMap dataMap = m_vpnSetting->data();

        ui->gateway->setText(dataMap.value("gateway"));
        ui->userName->setText(dataMap.value("user"));
        KLOG_DEBUG(qLcNetwork) << "password-flags:" << dataMap.value("password-flags");
        int index = ui->passwordOptions->findData(dataMap.value("password-flags"));
        if (index == -1)
        {
            // 代表password-flags为Setting::SecretFlagType::AgentOwned （0x01）
            // XXX:暂时不使用AgentOwned，统一改为None
            if (dataMap.value("password-flags") == 1)
            {
                int newIndex = ui->passwordOptions->findData(Setting::SecretFlagType::None);
                ui->passwordOptions->setCurrentIndex(newIndex);
            }
        }
        else
            ui->passwordOptions->setCurrentIndex(index);

        // 通过m_vpnSetting->secrets()获取到的map为空
        // NMStringMap secretMap = m_vpnSetting->secrets();
        // KLOG_DEBUG(qLcNetwork) << "vpn secretMap:" << secretMap;
        // KLOG_DEBUG(qLcNetwork) << "password:" << secretMap.value("password");
        // ui->password->setText(secretMap.value("password"));

        ui->ntDomain->setText(dataMap.value("domain"));

        // XXX:调用m_connection->secrets，会触发Connection::update，有待更改
        QDBusPendingReply<NMVariantMapMap> reply = m_connection->secrets("vpn");
        reply.waitForFinished();
        if (reply.isError() || !reply.isValid())
        {
            qDebug() << "get secrets error for connection:" << reply.error();
        }
        NMVariantMapMap NMVariantMap = reply.value();
        QVariantMap variantMap = NMVariantMap.value("vpn");
        QVariant secretsValue = variantMap.value("secrets");

        auto dbusArg = secretsValue.value<QDBusArgument>();
        KLOG_DEBUG(qLcNetwork) << dbusArg.currentType() << dbusArg.currentSignature();

        NMStringMap dbusMap = qdbus_cast<NMStringMap>(dbusArg);
        KLOG_DEBUG(qLcNetwork) << "dbusMap " << dbusMap;
        ui->password->setText(dbusMap.value("password"));
    }
    else
        resetSettings();
}

void VpnWidget::resetSettings()
{
    int index = ui->passwordOptions->findData(Setting::SecretFlagType::None);
    ui->passwordOptions->setCurrentIndex(index);
    ui->gateway->clear();
    ui->userName->clear();
    ui->password->clear();
    ui->ntDomain->clear();
    ui->password->setEchoMode(QLineEdit::Password);
}

void VpnWidget::clearPtr()
{
    m_vpnSetting.clear();
}

bool VpnWidget::isInputValid()
{
    QString gatewayStr = ui->gateway->text();
    if (gatewayStr.isEmpty())
    {
        QString error = QString(tr("Gateway can not be empty"));
        m_errorTip->setText(error);
        m_errorTip->showTipAroundWidget(ui->gateway);
        KLOG_DEBUG(qLcNetwork) << "Gateway cannot be empty";
        return false;
    }
    else
    {
        if (!isIpv4AddressValid(gatewayStr))
        {
            QString error = QString(tr("Gateway invalid"));
            m_errorTip->setText(error);
            m_errorTip->showTipAroundWidget(ui->gateway);
            KLOG_DEBUG(qLcNetwork) << "Gateway invalid";
            return false;
        }
    }

    if (ui->userName->text().isEmpty())
    {
        QString error = QString(tr("user name can not be empty"));
        m_errorTip->setText(error);
        m_errorTip->showTipAroundWidget(ui->userName);
        KLOG_DEBUG(qLcNetwork) << "user name can not be empty";
        return false;
    }

    if ((ui->passwordOptions->currentData().value<Setting::SecretFlagType>() == Setting::SecretFlagType::None) &&
        ui->password->text().isEmpty())
    {
        QString error = QString(tr("password can not be empty"));
        m_errorTip->setText(error);
        m_errorTip->showTipAroundWidget(ui->password);
        KLOG_DEBUG(qLcNetwork) << "password can not be empty";
        return false;
    }

    return true;
}

bool VpnWidget::isIpv4AddressValid(const QString &address)
{
    QHostAddress ipAddr(address);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv4) || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
    {
        return false;
    }
    QRegExp regExpIP("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
    return regExpIP.exactMatch(address);
}
