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

#include "vpn-page.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QScrollBar>
#include "ui_vpn-page.h"

//enum VpnType
//{
//    VPN_TYPE_L2TP,
//    VPN_TYPE_PPTP
//};

Q_DECLARE_METATYPE(VpnType)

VpnPage::VpnPage(QWidget *parent) : Page(parent), ui(new Ui::VpnPage)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

VpnPage::~VpnPage()
{
    delete ui;
}

void VpnPage::initUI()
{
    ui->connectionShowPage->setTitle(tr("VPN"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
    showVpnConnections();

    ui->vpnType->addItem(tr("L2TP"), VPN_TYPE_L2TP);
    ui->vpnType->addItem(tr("PPTP"), VPN_TYPE_PPTP);
}

//XXX:是否使用模板提升通用性
void VpnPage::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        //默认创建vpn类型：L2TP
        ui->vpnTypeWidget->setVisible(true);
        ui->vpnType->setCurrentIndex(VPN_TYPE_L2TP);
        ui->vpnTypeStacked->setCurrentIndex(VPN_TYPE_L2TP);
        ui->l2tpSetting->showSettingPage();

        QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
        scrollBar->setValue(0);
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestEditConnection, this, &VpnPage::handleRequestEditConnection);
    connect(ui->connectionShowPage, &ConnectionShowPage::requestActivateCurrentItemConnection, this, &VpnPage::handleActivateConnection);

    connect(ui->vpnType, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        VpnType type = ui->vpnType->currentData().value<VpnType>();
        ui->vpnTypeStacked->setCurrentIndex(type);
        switch (type)
        {
        case VPN_TYPE_L2TP:
            ui->l2tpSetting->showSettingPage();
            break;
        case VPN_TYPE_PPTP:
            ui->pptpSetting->showSettingPage();
            break;
        default:
            break;
        }
    });

    connect(ui->returnButton, &QPushButton::clicked, [=]() {
        clearVpnSetting();
        ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
    });

    connect(ui->l2tpSetting, &VpnL2tpSetting::settingUpdated, [=]() {
        ui->l2tpSetting->clearPtr();
        refreshConnectionLists();
    });

    connect(ui->pptpSetting, &VpnL2tpSetting::settingUpdated, [=]() {
        ui->pptpSetting->clearPtr();
        refreshConnectionLists();
    });

    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        int index = ui->vpnTypeStacked->currentIndex();
        switch (index)
        {
        case VPN_TYPE_L2TP:
            ui->l2tpSetting->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Vpn);
            break;
        case VPN_TYPE_PPTP:
            ui->pptpSetting->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Vpn);
            break;
        default:
            break;
        }
    });

    initNotifierConnection();
}

//XXX:待优化流程
void VpnPage::handleRequestEditConnection(const QString &uuid, QString activeConnectionPath)
{
    //隐藏选择VPN类型
    ui->vpnTypeWidget->setVisible(false);

    Connection::Ptr connection = findConnectionByUuid(uuid);
    ConnectionSettings::Ptr connectionSettings = connection->settings();
    VpnSetting::Ptr vpnSetting = connectionSettings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
    KLOG_DEBUG() << "vpnSetting->serviceType():" << vpnSetting->serviceType();
    QString serviceType = vpnSetting->serviceType();

    if (serviceType.contains("l2tp"))
    {
        ui->vpnTypeStacked->setCurrentIndex(VPN_TYPE_L2TP);
        ui->l2tpSetting->setConnection(connection);
        ui->l2tpSetting->setConnectionSettings(connectionSettings);

        ui->l2tpSetting->initSettingPage();
        ui->l2tpSetting->showSettingPage();
    }
    else if (serviceType.contains("pptp"))
    {
        ui->vpnTypeStacked->setCurrentIndex(VPN_TYPE_PPTP);
        ui->pptpSetting->setConnection(connection);
        ui->pptpSetting->setConnectionSettings(connectionSettings);

        ui->pptpSetting->initSettingPage();
        ui->pptpSetting->showSettingPage();
    }
    else
    {
        //other type
    }

    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

//考虑弹窗输入密码的情况
void VpnPage::handleActivateConnection(QString connectionPath)
{
    Connection::Ptr connection = findConnection(connectionPath);
    ConnectionSettings::Ptr settings = connection->settings();
    VpnSetting::Ptr vpnSetting = settings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();

    NMStringMap dataMap = vpnSetting->data();

    int passwordFlags = dataMap.value("password-flags").toInt();
    if(passwordFlags == Setting::SecretFlagType::None)
    {
        KLOG_DEBUG() << "passwordFlags == Setting::SecretFlagType::None";
        KLOG_DEBUG() << "dataMap.value(\"password\"):"  << dataMap.value("password");
    }
    else if (passwordFlags == Setting::SecretFlagType::NotSaved)
    {
        KLOG_DEBUG() << "flags == Setting::SecretFlagType::NotSaved";
    }
    else if(passwordFlags == Setting::SecretFlagType::NotRequired)
    {
        KLOG_DEBUG() << "passwordFlags == Setting::SecretFlagType::NotRequired";
    }


//    QDBusPendingReply<QDBusObjectPath> reply =
//        NetworkManager::activateConnection(connectionPath, "", "");
//
//    reply.waitForFinished();
//    if (reply.isError())
//    {
//        KLOG_DEBUG() << "activate connection failed" << reply.error();
//        //TODO:连接失败的处理
//    }
//    else
//    {
//        QString activatedPath = reply.value().path();
//        ActiveConnection::Ptr activatedConnectionObject = findActiveConnection(activatedPath);
//        ui->connectionShowPage->updateActivatedConnectionInfo(activatedPath);
//        ui->connectionShowPage->update();
//        KLOG_DEBUG() << "activate Connection successed";
//    }
}

void VpnPage::refreshConnectionLists()
{
    KLOG_DEBUG() << "VpnPage::refreshConnectionLists()";
    ui->connectionShowPage->clearConnectionLists();
    showVpnConnections();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void VpnPage::showVpnConnections()
{
    const Connection::List connectionList = NetworkManager::listConnections();
    for (Connection::Ptr conn : connectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::ConnectionType::Vpn)
        {
            KLOG_DEBUG() << "________________";
            KLOG_DEBUG() << "conn->uuid():" << conn->uuid();
            KLOG_DEBUG() << "conn->name():" << conn->name();
            KLOG_DEBUG() << "conn->path():" << conn->path();
            KLOG_DEBUG() << "----------------";
            ui->connectionShowPage->addConnectionToLists(conn);
        }
    }
}

void VpnPage::handleNotifierConnectionChanged()
{
    clearVpnSetting();
    refreshConnectionLists();
}

void VpnPage::clearVpnSetting()
{
    int index = ui->vpnTypeStacked->currentIndex();
    KLOG_DEBUG() << "vpnTypeStacked:" << index;
    switch (index)
    {
    case VPN_TYPE_L2TP:
        KLOG_DEBUG() << "ui->l2tpSetting->clearPtr";
        ui->l2tpSetting->clearPtr();
        break;
    case VPN_TYPE_PPTP:
        KLOG_DEBUG() << "ui->pptpSetting->clearPtr";
        ui->pptpSetting->clearPtr();
        break;
    default:
        break;
    }
}
