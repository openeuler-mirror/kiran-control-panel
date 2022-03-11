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

#include "comm-setting-widget.h"
#include <qt5-log-i.h>
#include "ui_comm-setting-widget.h"

using namespace NetworkManager;
CommSettingWidget::CommSettingWidget(QWidget *parent) : QWidget(parent), ui(new Ui::CommSettingWidget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

CommSettingWidget::~CommSettingWidget()
{
    delete ui;
}

void CommSettingWidget::initUI()
{
    ui->ipv4Method->addItem(tr("Auto"), Ipv4Setting::ConfigMethod::Automatic);
    ui->ipv4Method->addItem(tr("Manual"), Ipv4Setting::ConfigMethod::Manual);
    ui->ipv4Manual->setVisible(false);
}

void CommSettingWidget::initConnection()
{
    connect(ui->ipv4Method, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        handleIpv4MethodChanged(ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>());
    });
}

void CommSettingWidget::handleIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method)
{
    switch (method)
    {
    case Ipv4Setting::ConfigMethod::Automatic:
        ui->ipv4Manual->setVisible(false);
        KLOG_DEBUG() << "Automatic";
        break;
    case Ipv4Setting::ConfigMethod::Manual:
        ui->ipv4Manual->setVisible(true);
        KLOG_DEBUG() << "Manual";
        break;
    default:
        break;
    }
}

CommConfigInfo CommSettingWidget::getCommConfig()
{
    m_configInfo.ipv4Method = ui->ipv4Method->currentData().value<NetworkManager::Ipv4Setting::ConfigMethod>();
    m_configInfo.ipv4Address = ui->ipv4Address->text();
    m_configInfo.ipv4Netmask = ui->ipv4Netmask->text();
    m_configInfo.ipv4Gateway = ui->ipv4Gateway->text();
    return m_configInfo;
}

void CommSettingWidget::initCommConfig(CommConfigInfo configInfo)
{
    int index = ui->ipv4Method->findData(configInfo.ipv4Method);
    ui->ipv4Method->setCurrentIndex(index);
    ui->ipv4Address->setText(configInfo.ipv4Address);
    ui->ipv4Netmask->setText(configInfo.ipv4Netmask);
    ui->ipv4Gateway->setText(configInfo.ipv4Gateway);
}

void CommSettingWidget::initDisconnectAndDeleteButton(SettingConnectionStatus connectionStatus)
{
    switch (connectionStatus)
    {
    case SETTING_CONNECTION_STATUS_NEW:
        ui->deleteButton->setVisible(false);
        ui->disconnectButton->setVisible(false);
        break;
    case SETTING_CONNECTION_STATUS_DEACTIVATED:
        ui->deleteButton->setVisible(true);
        ui->disconnectButton->setVisible(false);
        break;
    case SETTING_CONNECTION_STATUS_ACTIVATED:
        ui->deleteButton->setVisible(true);
        ui->disconnectButton->setVisible(true);
        break;
    default:
        break;
    }
}
