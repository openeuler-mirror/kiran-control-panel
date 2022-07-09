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

#include "vpn-manager.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/VpnConnection>
#include <QDialog>
#include <QPointer>
#include <QScrollBar>
#include "text-input-dialog.h"
#include "ui_vpn-manager.h"
#include "status-notification.h"
#include "connection-name-widget.h"
#include <style-property.h>
Q_DECLARE_METATYPE(VpnType)

VpnManager::VpnManager(QWidget *parent) : Manager(parent), ui(new Ui::VpnManager)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

VpnManager::~VpnManager()
{
    delete ui;
}

void VpnManager::initUI()
{
    ui->connectionShowPage->setConnectionType(ConnectionSettings::Vpn);
    ui->connectionShowPage->setItemWidgetType(ITEM_WIDGET_TYPE_PLUGIN);
    ui->connectionShowPage->setTitle(tr("VPN"));
    ui->connectionShowPage->setSwitchButtonVisible(false);
    ui->connectionShowPage->showConnectionLists(ConnectionSettings::Vpn);

//    ui->vpnType->addItem(tr("L2TP"), VPN_TYPE_L2TP);
    ui->vpnType->addItem(tr("PPTP"), VPN_TYPE_PPTP);
    Kiran::StylePropertyHelper::setButtonType(ui->saveButton,Kiran::BUTTON_Default);
}

//XXX:是否使用模板提升通用性
void VpnManager::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        //默认创建vpn类型：L2TP
        ui->vpnTypeWidget->setVisible(true);
        ui->vpnType->setCurrentIndex(0);
        ui->vpnTypeStacked->setCurrentIndex(VPN_TYPE_PPTP);
        ui->pptpSetting->showSettingPage();

        QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
        scrollBar->setValue(0);
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestEditConnection, this, &VpnManager::handleRequestEditConnection);
    connect(ui->connectionShowPage, &ConnectionShowPage::requestActivateCurrentItemConnection, this, &VpnManager::handleRequestActivateConnection);

    connect(ui->vpnType, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        VpnType type = ui->vpnType->currentData().value<VpnType>();
        ui->vpnTypeStacked->setCurrentIndex(VPN_TYPE_PPTP);
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

    connect(ui->returnButton, &QPushButton::clicked, this, &VpnManager::handleReturnPreviousPage);
    connect(ui->l2tpSetting, &VpnL2tpSetting::returnPreviousPage, this, &VpnManager::handleReturnPreviousPage);
    connect(ui->pptpSetting, &VpnPptpSetting::returnPreviousPage, this, &VpnManager::handleReturnPreviousPage);

    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        int index = ui->vpnTypeStacked->currentIndex();
        switch (index)
        {
        case VPN_TYPE_L2TP:
            if(ui->l2tpSetting->isInputValid())
            {
                ui->l2tpSetting->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Vpn);
                handleReturnPreviousPage();
            }
            break;
        case VPN_TYPE_PPTP:
            if(ui->pptpSetting->isInputValid())
            {
                ui->pptpSetting->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Vpn);
                handleReturnPreviousPage();
            }
            break;
        default:
            break;
        }
    });

    connect(ui->connectionShowPage,&ConnectionShowPage::connectionUpdated,[=](const QString &path){
        KLOG_DEBUG() << "Connection::updated:" << path;
        //移除后再加载进来以更新信息
        ui->connectionShowPage->removeConnectionFromLists(path);
        Connection::Ptr updateConnection = findConnection(path);
        ui->connectionShowPage->addConnectionToLists(updateConnection,"");
        handleReturnPreviousPage();
    });

    initNotifierConnection();
}


void VpnManager::handleRequestEditConnection(const QString &uuid, QString activeConnectionPath)
{
    //隐藏选择VPN类型
    ui->vpnTypeWidget->setVisible(false);

    Connection::Ptr connection = findConnectionByUuid(uuid);
    ConnectionSettings::Ptr connectionSettings = connection->settings();
    VpnSetting::Ptr vpnSetting = connectionSettings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
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
void VpnManager::handleRequestActivateConnection(const QString &connectionPath,const QString &connectionParameter)
{
    Connection::Ptr connection = findConnection(connectionPath);
    ConnectionSettings::Ptr settings = connection->settings();
    VpnSetting::Ptr vpnSetting = settings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
    NMStringMap dataMap = vpnSetting->data();

    int passwordFlags = dataMap.value("password-flags").toInt();
    if (passwordFlags == Setting::SecretFlagType::None)
    {
        activateVPNConnection(connectionPath, connectionParameter);
    }
    else if (passwordFlags == Setting::SecretFlagType::NotRequired)
    {
        activateVPNConnection(connectionPath, connectionParameter);
    }
    else if (passwordFlags == Setting::SecretFlagType::NotSaved)
    {
        TextInputDialog inputDialog;
        inputDialog.setTitle(tr("Tips"));
        QString tips = QString(tr("Password required to connect to %1.")).arg(settings->id());
        inputDialog.setText(tips);

        connect(&inputDialog, &TextInputDialog::password, [=](const QString &password) {
            NMStringMap secretsMap = vpnSetting->secrets();
            secretsMap.insert("password", password);
            vpnSetting->setSecrets(secretsMap);
            activateVPNConnection(connectionPath, connectionParameter);
            connection->clearSecrets();
        });

        inputDialog.exec();
    }
}

void VpnManager::activateVPNConnection(const QString &connectionPath,const QString &connectionParameter)
{
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, "", connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_ERROR() << "activate connection failed" << reply.error();
        StatusNotification::connectitonFailedNotify(connectionPath);
    }
    else
    {
        KLOG_DEBUG() << "reply.reply():" << reply.reply();
        QString activatedPath = reply.value().path();
    }
}

void VpnManager::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection =  findConnection(path);
    if ((connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Vpn)
        && (!connection->name().isEmpty()))
    {
        ui->connectionShowPage->addConnectionToLists(connection,"");
    }
}

void VpnManager::handleNotifierConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->removeConnectionFromLists(path);
}

void VpnManager::handleActiveConnectionAdded(const QString &activePath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activePath);
    if(activatedConnection == nullptr)
    {
        KLOG_DEBUG() << "activatedConnection == nullptr";
        return ;
    }
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Vpn)
    {
        VpnConnection::Ptr vpnConnection = findActiveConnection(activePath).dynamicCast<VpnConnection>();
        QString uuid = vpnConnection->uuid();
        KLOG_DEBUG() << "vpn uuid:" << uuid;
        QListWidgetItem *activeItem = ui->connectionShowPage->findItemByUuid(uuid);
        ui->connectionShowPage->updateItemActivatedPath(activeItem,activePath);
        connect(vpnConnection.data(), &VpnConnection::stateChanged, [=](VpnConnection::State state, VpnConnection::StateChangeReason reason) {
            handleVpnConnectionStateChanged(state, reason, activePath);
        });
        //加载等待动画
        auto item = ui->connectionShowPage->findItemByActivatedPath(activePath);
        ui->connectionShowPage->updateItemActivatingStatus(item);
    }
}

void VpnManager::handleActiveConnectionRemoved(const QString &activePath)
{
}

//TODO:若没有安装VPN插件则需要提示
void VpnManager::handleVpnConnectionStateChanged(VpnConnection::State state, VpnConnection::StateChangeReason reason, const QString &activePath)
{
    auto activeConnection = findActiveConnection(activePath);
    KLOG_DEBUG()  << " activeConnection->id():" <<  activeConnection->id();
    QString id = "";
    if(activeConnection != nullptr)
        id = activeConnection->id();
    switch (state)
    {
    case VpnConnection::State::Unknown:
        KLOG_DEBUG() << "VpnConnection::State::Unknown";
        break;
    case VpnConnection::State::Prepare:
        KLOG_DEBUG() << "VpnConnection::State::Prepare";
        break;
    case VpnConnection::State::NeedAuth:
        KLOG_DEBUG() << "VpnConnection::State::NeedAuth";
        break;
    case VpnConnection::State::Connecting:
        KLOG_DEBUG() << "VpnConnection::State::Connecting";
        break;
    case VpnConnection::State::GettingIpConfig:
        KLOG_DEBUG() << "VpnConnection::State::GettingIpConfig";
        break;
    case VpnConnection::State::Activated:
        KLOG_DEBUG() << "VpnConnection::State::Activated";
        handleVpnStateActivated(activePath);
        break;
    case VpnConnection::State::Failed:
        KLOG_DEBUG() << "VpnConnection::State::Failed";
        if(!id.isEmpty())
            StatusNotification::ActiveConnectionDeactivatedNotify(id);
        handleVpnStateFailed(activePath);
        break;
    case VpnConnection::State::Disconnected:
        KLOG_DEBUG() << "VpnConnection::State::Disconnected";
        if(!id.isEmpty())
            StatusNotification::ActiveConnectionDeactivatedNotify(id);
        handleVpnStateDisconnected(activePath);
        break;
    default:
        break;
    }

    switch (reason)
    {
    case VpnConnection::StateChangeReason::UnknownReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::UnknownReason";
        break;
    case VpnConnection::StateChangeReason::NoneReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::NoneReason";
        break;
    case VpnConnection::StateChangeReason::UserDisconnectedReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::UserDisconnectedReason";
        break;
    case VpnConnection::StateChangeReason::DeviceDisconnectedReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::DeviceDisconnectedReason";
        break;
    case VpnConnection::StateChangeReason::ServiceStoppedReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::ServiceStoppedReason";
        break;
    case VpnConnection::StateChangeReason::IpConfigInvalidReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::IpConfigInvalidReason";
        break;
    case VpnConnection::StateChangeReason::ConnectTimeoutReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::ConnectTimeoutReason";
        break;
    case VpnConnection::StateChangeReason::ServiceStartTimeoutReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::ServiceStartTimeoutReason";
        break;
    case VpnConnection::StateChangeReason::ServiceStartFailedReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::ServiceStartFailedReason";
        break;
    case VpnConnection::StateChangeReason::NoSecretsReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::NoSecretsReason";
        break;
    case VpnConnection::StateChangeReason::LoginFailedReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::LoginFailedReason";
        break;
    case VpnConnection::StateChangeReason::ConnectionRemovedReason:
        KLOG_DEBUG() << "VpnConnection::StateChangeReason::ConnectionRemovedReason";
        break;
    default:
        break;
    }
}

void VpnManager::handleVpnStateActivated(const QString &activePath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activePath);
    if(activeConnection.isNull())
        return ;
    if( activeConnection->type() == ConnectionSettings::Vpn)
    {
        ui->connectionShowPage->updateItemActivatedStatus(activePath);
        auto item = ui->connectionShowPage->findItemByActivatedPath(activePath);
        NetworkConnectionInfo connectionInfo = item->data(Qt::UserRole).value<NetworkConnectionInfo>();
        StatusNotification::ActiveConnectionActivatedNotify(connectionInfo);

        ui->connectionShowPage->update();
    }
}

void VpnManager::handleVpnStateDisconnected(const QString &activePath)
{
    ui->connectionShowPage->handleActiveStateDeactivated(activePath);
}

void VpnManager::handleVpnStateFailed(const QString &activePath)
{
    ui->connectionShowPage->handleActiveStateDeactivated(activePath);
}

void VpnManager::handleReturnPreviousPage()
{
    clearVpnSetting();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void VpnManager::clearVpnSetting()
{
    int index = ui->vpnTypeStacked->currentIndex();
    switch (index)
    {
    case VPN_TYPE_L2TP:
        ui->l2tpSetting->clearPtr();
        break;
    case VPN_TYPE_PPTP:
        ui->pptpSetting->clearPtr();
        break;
    default:
        break;
    }
}

