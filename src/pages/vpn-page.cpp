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
#include <NetworkManagerQt/VpnConnection>
#include <QDialog>
#include <QScrollBar>
#include "ui_vpn-page.h"
#include "widgets/input-dialog.h"

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
    connect(ui->connectionShowPage, &ConnectionShowPage::requestActivateCurrentItemConnection, this, &VpnPage::handleRequestActivateConnection);

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

    connect(ui->returnButton, &QPushButton::clicked, this, &VpnPage::handleReturnPreviousPage);
    connect(ui->l2tpSetting, &VpnL2tpSetting::returnPreviousPage, this, &VpnPage::handleReturnPreviousPage);
    connect(ui->pptpSetting, &VpnPptpSetting::returnPreviousPage, this, &VpnPage::handleReturnPreviousPage);

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
        handleReturnPreviousPage();
    });

    connect(ui->l2tpSetting, &VpnL2tpSetting::settingUpdated, [=]() {
        ui->l2tpSetting->clearPtr();
        ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
        refreshConnectionLists();
    });

    connect(ui->pptpSetting, &VpnL2tpSetting::settingUpdated, [=]() {
        ui->pptpSetting->clearPtr();
        ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
        refreshConnectionLists();
    });

    initNotifierConnection();
}

void VpnPage::showVpnConnections()
{
    const Connection::List connectionList = NetworkManager::listConnections();
    for (Connection::Ptr conn : connectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::ConnectionType::Vpn)
        {
            //            KLOG_DEBUG() << "________________";
            //            KLOG_DEBUG() << "conn->uuid():" << conn->uuid();
            //            KLOG_DEBUG() << "conn->name():" << conn->name();
            //            KLOG_DEBUG() << "conn->path():" << conn->path();
            //            KLOG_DEBUG() << "----------------";
            ui->connectionShowPage->addConnectionToLists(conn);
        }
    }
}

void VpnPage::handleRequestEditConnection(const QString &uuid, QString activeConnectionPath)
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
void VpnPage::handleRequestActivateConnection(QString connectionPath)
{
    Connection::Ptr connection = findConnection(connectionPath);
    ConnectionSettings::Ptr settings = connection->settings();
    VpnSetting::Ptr vpnSetting = settings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
    NMStringMap dataMap = vpnSetting->data();

    int passwordFlags = dataMap.value("password-flags").toInt();
    if (passwordFlags == Setting::SecretFlagType::None)
    {
        activatingConnection(connectionPath);
    }
    else if (passwordFlags == Setting::SecretFlagType::NotRequired)
    {
        activatingConnection(connectionPath);
    }
    else if (passwordFlags == Setting::SecretFlagType::NotSaved)
    {
        InputDialog inputDialog;
        inputDialog.setTitle(tr("Tips"));
        QString tips = QString(tr("Password required to connect to %1.")).arg(settings->id());
        inputDialog.setText(tips);

        connect(&inputDialog, &InputDialog::password, [=](const QString &password) {
            NMStringMap secretsMap = vpnSetting->secrets();
            KLOG_DEBUG() << "password:" << password;
            secretsMap.insert("password", password);
            KLOG_DEBUG() << "secretsMap:" << secretsMap;
            vpnSetting->setSecrets(secretsMap);

            activatingConnection(connectionPath);
            connection->clearSecrets();
        });

        inputDialog.exec();
    }
}

void VpnPage::activatingConnection(const QString &connectionPath)
{
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, "", "");

    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_DEBUG() << "activate connection failed" << reply.error();
        //TODO:连接失败的处理
    }
    else
    {
        KLOG_DEBUG() << "reply.reply():" << reply.reply();
        QString activatedPath = reply.value().path();
    }
}

void VpnPage::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection =  findConnection(path);
    if (connection->settings()->connectionType() == ConnectionSettings::ConnectionType::Vpn)
    {
        ui->connectionShowPage->addConnectionToLists(connection);
    }
}

//remove时不能再创建连接对象，因为连接可能已经不存在了
void VpnPage::handleNotifierConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->removeConnectionFromLists(path);
}

void VpnPage::handleActiveConnectionAdded(const QString &activePath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activePath);
    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Vpn)
    {
        VpnConnection::Ptr vpnConnection = findActiveConnection(activePath).dynamicCast<VpnConnection>();
        QString uuid = vpnConnection->uuid();
        KLOG_DEBUG() << "vpn uuid:" << uuid;
        ui->connectionShowPage->findItemByUuid(uuid);
        connect(vpnConnection.data(), &VpnConnection::stateChanged, [=](VpnConnection::State state, VpnConnection::StateChangeReason reason) {
            handleVpnConnectionStateChanged(state, reason, activePath);
        });
        //加载等待动画
        ui->connectionShowPage->connectionItemLoadingAnimation();
    }
}

void VpnPage::handleActiveConnectionRemoved(const QString &activePath)
{
    KLOG_DEBUG() << "handleActiveConnectionRemoved";
}

void VpnPage::handleVpnConnectionStateChanged(VpnConnection::State state, VpnConnection::StateChangeReason reason, const QString &activePath)
{
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
        handleVpnStateFailed(activePath);
        break;
    case VpnConnection::State::Disconnected:
        KLOG_DEBUG() << "VpnConnection::State::Disconnected";
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

void VpnPage::handleVpnStateActivated(const QString &activePath)
{
    ui->connectionShowPage->updateActivatedConnectionInfo(activePath);
    ui->connectionShowPage->update();
}

void VpnPage::handleVpnStateDisconnected(const QString &activePath)
{
    ui->connectionShowPage->clearDeactivatedConnectionInfo(activePath);
    ui->connectionShowPage->update();
}
//预留，暂时不需要区分failed和disconnected
void VpnPage::handleVpnStateFailed(const QString &activePath)
{
    ui->connectionShowPage->clearDeactivatedConnectionInfo(activePath);
    ui->connectionShowPage->update();
}

void VpnPage::handleReturnPreviousPage()
{
    clearVpnSetting();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void VpnPage::refreshConnectionLists()
{
    KLOG_DEBUG() << "VpnPage::refreshConnectionLists()";
    ui->connectionShowPage->clearConnectionLists();
    showVpnConnections();
}

void VpnPage::clearVpnSetting()
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

