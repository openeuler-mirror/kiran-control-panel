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

#include "wireless-security-widget.h"
#include <NetworkManagerQt/Setting>
#include "ui_wireless-security-widget.h"
using namespace NetworkManager;


Q_DECLARE_METATYPE(Setting::SecretFlagType)
WirelessSecurityWidget::WirelessSecurityWidget(QWidget *parent) : QWidget(parent), ui(new Ui::WirelessSecurityWidget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

WirelessSecurityWidget::~WirelessSecurityWidget()
{
    delete ui;
}

void WirelessSecurityWidget::initUI()
{
    ui->securityOption->addItem(tr("None"), WirelessSecuritySetting::KeyMgmt::WpaNone);
    ui->securityOption->addItem(tr("WPA/WPA2 Personal"), WirelessSecuritySetting::KeyMgmt::WpaPsk);
    int defalutIndex = ui->securityOption->findData(WirelessSecuritySetting::KeyMgmt::WpaPsk);
    ui->securityOption->setCurrentIndex(defalutIndex);

    ui->passwordOption->addItem(tr("Save password for all users"), Setting::SecretFlagType::None);
    ui->passwordOption->addItem(tr("Save password for this user"), Setting::SecretFlagType::AgentOwned);
    ui->passwordOption->addItem(tr("Ask me always"), Setting::SecretFlagType::NotSaved);

    ui->passwordEdit->setPlaceholderText(tr("Required"));
    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    //Note:暂时不需要在该处输入和显示密码的功能，暂时隐藏
    ui->passwordWidget->setVisible(false);
}

void WirelessSecurityWidget::initConnection()
{
    connect(ui->securityOption, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        handleSecurityOptionChanged(ui->securityOption->currentData().value<WirelessSecuritySetting::KeyMgmt>());
    });
//    connect(ui->passwordOption, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
//        handlePasswordOptionsChanged(ui->passwordOption->currentData().value<Setting::SecretFlagType>());
//    });
    connect(ui->passwordVisual, &QPushButton::clicked, this, &WirelessSecurityWidget::enablePasswordVisual);
}

void WirelessSecurityWidget::handleSecurityOptionChanged(WirelessSecuritySetting::KeyMgmt keyMgmt)
{
    switch (keyMgmt)
    {
    case WirelessSecuritySetting::KeyMgmt::WpaNone:
        ui->stackedWidget->setVisible(false);
        break;
    case WirelessSecuritySetting::KeyMgmt::WpaPsk:
        ui->stackedWidget->setVisible(true);
        //待修改枚举值
        ui->stackedWidget->setCurrentIndex(0);
        break;
    default:
        break;
    }
}

void WirelessSecurityWidget::handlePasswordOptionsChanged(Setting::SecretFlagType secretFlagType)
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
    default:
        break;
    }
}

void WirelessSecurityWidget::setWirelessSecuritySetting(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting)
{
    m_wirelessSecuritySetting = wirelessSecuritySetting;
}

void WirelessSecurityWidget::saveSettings()
{
    WirelessSecuritySetting::KeyMgmt currentKeyMgmt = ui->securityOption->currentData().value<WirelessSecuritySetting::KeyMgmt>();
    m_wirelessSecuritySetting->setKeyMgmt(currentKeyMgmt);

    if (currentKeyMgmt == WirelessSecuritySetting::WpaNone || currentKeyMgmt == WirelessSecuritySetting::Unknown)
    {
        return m_wirelessSecuritySetting->setInitialized(false);
    }
    else if (currentKeyMgmt == WirelessSecuritySetting::WpaPsk)
    {
        Setting::SecretFlagType cureentSecretFlag = ui->passwordOption->currentData().value<Setting::SecretFlagType>();
        m_wirelessSecuritySetting->setPskFlags(cureentSecretFlag);
        if (cureentSecretFlag != NetworkManager::Setting::NotSaved)
        {
            m_wirelessSecuritySetting->setPsk(ui->passwordEdit->text());
        }
        else
        {
            m_wirelessSecuritySetting->setPsk(QString());
        }
        m_wirelessSecuritySetting->setWepKeyType(WirelessSecuritySetting::WepKeyType::NotSpecified);
        m_wirelessSecuritySetting->setWepKeyFlags(Setting::NotRequired);
    }

    m_wirelessSecuritySetting->setInitialized(true);
    //待添加
//    wirelessSetting->setSecurity("802-11-wireless-security");
}

void WirelessSecurityWidget::showSettings()
{
    if (m_wirelessSecuritySetting != nullptr)
    {
        WirelessSecuritySetting::KeyMgmt keyMgmt = m_wirelessSecuritySetting->keyMgmt();
        int index = ui->securityOption->findData(keyMgmt);
        ui->securityOption->setCurrentIndex(index);

        if (keyMgmt == WirelessSecuritySetting::KeyMgmt::WpaNone)
            ui->stackedWidget->setVisible(false);
        else if (WirelessSecuritySetting::KeyMgmt::WpaPsk)
        {
            ui->stackedWidget->setVisible(true);
            //待修改枚举值
            ui->stackedWidget->setCurrentIndex(0);
            Setting::SecretFlags pskFlags = m_wirelessSecuritySetting->pskFlags();
            int secretFlagIndex;
            if (pskFlags == Setting::SecretFlagType::NotSaved)
            {
                secretFlagIndex = ui->passwordOption->findData(Setting::NotSaved);
                ui->passwordOption->setCurrentIndex(secretFlagIndex);
                ui->passwordWidget->setVisible(false);
                return;
            }

            if (pskFlags == Setting::SecretFlagType::AgentOwned)
            {
                secretFlagIndex = ui->passwordOption->findData(Setting::AgentOwned);
            }
            else if (pskFlags == Setting::SecretFlagType::NotRequired)
            {
                secretFlagIndex = ui->passwordOption->findData(Setting::NotRequired);
            }
            //暂时隐藏passwordWidget
//            ui->passwordWidget->setVisible(true);
            ui->passwordOption->setCurrentIndex(secretFlagIndex);
            ui->passwordEdit->setText(m_wirelessSecuritySetting->psk());
        }
    }
    else
        resetSettings();
}

void WirelessSecurityWidget::clearPtr()
{
    m_wirelessSecuritySetting.clear();
}

void WirelessSecurityWidget::enablePasswordVisual()
{
    if (ui->passwordEdit->echoMode() == QLineEdit::Password)
    {
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
    }
}

void WirelessSecurityWidget::resetSettings()
{
    int index;
    index = ui->securityOption->findData(WirelessSecuritySetting::KeyMgmt::WpaPsk);
    ui->securityOption->setCurrentIndex(index);
    index = ui->passwordOption->findData(Setting::None);
    ui->passwordOption->setCurrentIndex(index);
    //暂时隐藏passwordWidget
//    ui->passwordWidget->setVisible(true);
    ui->passwordEdit->clear();
}
