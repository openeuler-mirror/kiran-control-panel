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

#include "vpn-manager.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/VpnConnection>
#include <QDialog>
#include <QPointer>
#include <QScrollBar>
#include "connection-name-widget.h"
#include "logging-category.h"
#include "signal-forward.h"
#include "status-notification.h"
#include "text-input-dialog.h"
#include "ui_vpn-manager.h"
using namespace NetworkManager;

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
    ui->connectionShowPage->init(ConnectionSettings::Vpn, "");
    ui->connectionShowPage->setTitle(tr("VPN"));
    ui->connectionShowPage->setSwitchButtonVisible(false);

    ui->vpnType->addItem(tr("L2TP"), VPN_TYPE_L2TP);
    // ui->vpnType->addItem(tr("PPTP"), VPN_TYPE_PPTP);
    // FIXME: 后续使用新版kiran-integration-qt5中提供的setButtonType函数
    // Kiran::StylePropertyHelper::setButtonType(ui->saveButton, Kiran::BUTTON_Default);
}

// XXX:是否使用模板提升通用性
void VpnManager::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::creatConnection, this, &VpnManager::handleCreatConnection);
    connect(ui->connectionShowPage, &ConnectionShowPage::editConnection, this, &VpnManager::handleEditConnection);
    connect(ui->connectionShowPage, &ConnectionShowPage::activateSelectedConnection, this, &VpnManager::handleActivateSelectedConnection);

    connect(ui->vpnType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
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
                } });

    connect(ui->returnButton, &QPushButton::clicked, this, &VpnManager::handleReturnPreviousPage);
    connect(ui->saveButton, &QPushButton::clicked, this, &VpnManager::handleSaveButtonClicked);

    connect(ui->l2tpSetting, &VpnL2tpSetting::returnPreviousPage, this, &VpnManager::handleReturnPreviousPage);
    connect(ui->pptpSetting, &VpnPptpSetting::returnPreviousPage, this, &VpnManager::handleReturnPreviousPage);

    connect(ui->connectionShowPage, &ConnectionShowPage::connectionUpdated, this, &VpnManager::handleConnectionUpdated);

    connect(SignalForward::instance(), &SignalForward::vpnConnectionAdded, this, &VpnManager::handleNotifierConnectionAdded);
    connect(SignalForward::instance(), &SignalForward::vpnActiveConnectionAdded, this, &VpnManager::handleActiveConnectionAdded);

    connect(SignalForward::instance(), &SignalForward::connectionRemoved, this, &VpnManager::handleNotifierConnectionRemoved);
    connect(SignalForward::instance(), &SignalForward::activeConnectionRemoved, this, &VpnManager::handleActiveConnectionRemoved);
}

void VpnManager::handleCreatConnection()
{
    // 默认创建vpn类型：L2TP
    ui->vpnTypeWidget->setVisible(true);
    ui->vpnType->setCurrentIndex(VPN_TYPE_L2TP);
    ui->vpnTypeStacked->setCurrentIndex(VPN_TYPE_L2TP);
    ui->l2tpSetting->showSettingPage();
    // ui->vpnType->setCurrentIndex(0);

    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void VpnManager::handleEditConnection(const QString &uuid, QString activeConnectionPath)
{
    // 隐藏选择VPN类型
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
        ui->l2tpSetting->showSettingPage(activeConnectionPath);
    }
    else if (serviceType.contains("pptp"))
    {
        ui->vpnTypeStacked->setCurrentIndex(VPN_TYPE_PPTP);
        ui->pptpSetting->setConnection(connection);
        ui->pptpSetting->setConnectionSettings(connectionSettings);

        ui->pptpSetting->initSettingPage();
        ui->pptpSetting->showSettingPage(activeConnectionPath);
    }
    else
    {
        // other type
    }

    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void VpnManager::handleSaveButtonClicked()
{
    int index = ui->vpnTypeStacked->currentIndex();
    switch (index)
    {
    case VPN_TYPE_L2TP:
        if (ui->l2tpSetting->isInputValid())
        {
            ui->l2tpSetting->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Vpn);
            handleReturnPreviousPage();
        }
        break;
    case VPN_TYPE_PPTP:
        if (ui->pptpSetting->isInputValid())
        {
            ui->pptpSetting->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Vpn);
            handleReturnPreviousPage();
        }
        break;
    default:
        break;
    }
}

// 考虑弹窗输入密码的情况
//  TODO: AgentOwned的作用
void VpnManager::handleActivateSelectedConnection(const QString &connectionPath, const QString &connectionParameter)
{
    Connection::Ptr connection = findConnection(connectionPath);
    ConnectionSettings::Ptr settings = connection->settings();
    VpnSetting::Ptr vpnSetting = settings->setting(Setting::SettingType::Vpn).dynamicCast<VpnSetting>();
    NMStringMap dataMap = vpnSetting->data();

    int passwordFlags = dataMap.value("password-flags").toInt();
    /**
     * None - 已保存的 Save password for all users
     * NotRequired - 不要求
     * NotSaved - 总是询问
     * AgentOwned - 暂时不使用，Save password for this user
     */
    if (passwordFlags == Setting::SecretFlagType::None || passwordFlags == Setting::SecretFlagType::AgentOwned)
    {
        activateVPNConnection(connectionPath, connectionParameter);
        KLOG_DEBUG(qLcNetwork) << "passwordFlags None";
    }
    else if (passwordFlags == Setting::SecretFlagType::NotRequired)
    {
        activateVPNConnection(connectionPath, connectionParameter);
        KLOG_DEBUG(qLcNetwork) << "passwordFlags NotRequired";
    }
    else if (passwordFlags == Setting::SecretFlagType::NotSaved)
    {
        KLOG_DEBUG(qLcNetwork) << "passwordFlags NotSaved";
        TextInputDialog inputDialog;
        inputDialog.setTitle(tr("Tips"));
        QString tips = QString(tr("Password required to connect to %1.")).arg(settings->id());
        inputDialog.setText(tips);
        inputDialog.setlineEditEchoMode(QLineEdit::Password);
        connect(&inputDialog, &TextInputDialog::password, this, [=](const QString &password)
                {
                    NMStringMap secretsMap = vpnSetting->secrets();
                    secretsMap.insert("password", password);
                    vpnSetting->setSecrets(secretsMap);
                    activateVPNConnection(connectionPath, connectionParameter);
                    connection->clearSecrets(); });

        inputDialog.exec();
    }
}

void VpnManager::activateVPNConnection(const QString &connectionPath, const QString &connectionParameter)
{
    QDBusPendingReply<QDBusObjectPath> reply =
        NetworkManager::activateConnection(connectionPath, "", connectionParameter);

    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_ERROR(qLcNetwork) << "activate connection failed" << reply.error();
        StatusNotification::connectitonFailedNotify(connectionPath);
    }
    else
    {
        KLOG_DEBUG(qLcNetwork) << "reply.reply():" << reply.reply();
    }
}

void VpnManager::handleNotifierConnectionAdded(const QString &path)
{
    Connection::Ptr connection = findConnection(path);
    ui->connectionShowPage->addConnection(connection, "");
}

void VpnManager::handleNotifierConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->removeConnectionFromList(path);
}

void VpnManager::handleActiveConnectionAdded(const QString &activePath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activePath);
    if (activatedConnection == nullptr)
    {
        KLOG_DEBUG(qLcNetwork) << "activatedConnection == nullptr";
        return;
    }

    VpnConnection::Ptr vpnConnection = findActiveConnection(activePath).dynamicCast<VpnConnection>();
    if (vpnConnection.isNull())
    {
        return;
    }
    QString uuid = vpnConnection->uuid();
    KLOG_DEBUG(qLcNetwork) << "vpn uuid:" << uuid;
    QWidget *activeItemWidget = ui->connectionShowPage->findItemWidgetByUuid(uuid);
    if (activeItemWidget != nullptr)
    {
        ui->connectionShowPage->updateItemWidgetActivePath(activeItemWidget, activePath);
    }
    connect(vpnConnection.data(), &VpnConnection::stateChanged, this, &VpnManager::handleVpnConnectionStateChanged, Qt::UniqueConnection);
}

void VpnManager::handleActiveConnectionRemoved(const QString &activePath)
{
}

// TODO:若没有安装VPN插件则需要提示
void VpnManager::handleVpnConnectionStateChanged(VpnConnection::State state, VpnConnection::StateChangeReason reason)
{
    // auto activeConnection = qobject_cast<ActiveConnection *>(sender());

    auto activeVpnConnection = qobject_cast<VpnConnection *>(sender());
    QString activePath = activeVpnConnection->path();
    KLOG_DEBUG(qLcNetwork) << " activeConnection->id():" << activeVpnConnection->id();
    QString id = "";
    if (activeVpnConnection != nullptr)
        id = activeVpnConnection->id();
    switch (state)
    {
    case VpnConnection::State::Unknown:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::Unknown";
        break;
    case VpnConnection::State::Prepare:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::Prepare";
        break;
    case VpnConnection::State::NeedAuth:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::NeedAuth";
        break;
    case VpnConnection::State::Connecting:
        handleStateActivating(activePath);
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::Connecting";
        break;
    case VpnConnection::State::GettingIpConfig:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::GettingIpConfig";
        break;
    case VpnConnection::State::Activated:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::Activated";
        handleStateActivated(activePath);
        break;
    case VpnConnection::State::Failed:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::Failed";
        if (!id.isEmpty())
            StatusNotification::ActiveConnectionDeactivatedNotify(id);
        handleVpnStateFailed(activePath);
        break;
    case VpnConnection::State::Disconnected:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::State::Disconnected";
        if (!id.isEmpty())
            StatusNotification::ActiveConnectionDeactivatedNotify(id);
        handleVpnStateDisconnected(activePath);
        break;
    default:
        break;
    }

    switch (reason)
    {
    case VpnConnection::StateChangeReason::UnknownReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::UnknownReason";
        break;
    case VpnConnection::StateChangeReason::NoneReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::NoneReason";
        break;
    case VpnConnection::StateChangeReason::UserDisconnectedReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::UserDisconnectedReason";
        break;
    case VpnConnection::StateChangeReason::DeviceDisconnectedReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::DeviceDisconnectedReason";
        break;
    case VpnConnection::StateChangeReason::ServiceStoppedReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::ServiceStoppedReason";
        break;
    case VpnConnection::StateChangeReason::IpConfigInvalidReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::IpConfigInvalidReason";
        break;
    case VpnConnection::StateChangeReason::ConnectTimeoutReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::ConnectTimeoutReason";
        break;
    case VpnConnection::StateChangeReason::ServiceStartTimeoutReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::ServiceStartTimeoutReason";
        break;
    case VpnConnection::StateChangeReason::ServiceStartFailedReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::ServiceStartFailedReason";
        break;
    case VpnConnection::StateChangeReason::NoSecretsReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::NoSecretsReason";
        break;
    case VpnConnection::StateChangeReason::LoginFailedReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::LoginFailedReason";
        break;
    case VpnConnection::StateChangeReason::ConnectionRemovedReason:
        KLOG_DEBUG(qLcNetwork) << "VpnConnection::StateChangeReason::ConnectionRemovedReason";
        break;
    default:
        break;
    }
}

void VpnManager::handleStateActivated(const QString &activePath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activePath);
    if (activeConnection.isNull())
        return;
    if (activeConnection->type() == ConnectionSettings::Vpn)
    {
        ui->connectionShowPage->setItemWidgetStatus(activePath, ActiveConnection::State::Activated);
        ui->connectionShowPage->sort();
        auto itemWidget = ui->connectionShowPage->findItemWidgetByActivePath(activePath);
        if (itemWidget != nullptr)
        {
            StatusNotification::ActiveConnectionActivatedNotify(activeConnection->id());
            ui->connectionShowPage->update();
        }
    }
}

void VpnManager::handleStateActivating(const QString &activePath)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(activePath);
    if (activatedConnection.isNull())
        return;

    if (activatedConnection->type() == ConnectionSettings::ConnectionType::Vpn)
    {
        // 加载等待动画
        ui->connectionShowPage->setItemWidgetStatus(activePath, ActiveConnection::State::Activating);
    }
}

void VpnManager::handleStateDeactivated(const QString &deactivatedPath)
{
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

// TODO:更新列表逻辑需要修改
void VpnManager::handleConnectionUpdated(const QString &path)
{
    KLOG_DEBUG(qLcNetwork) << "Connection::updated:" << path;
    Connection::Ptr updateConnection = findConnection(path);
    if (updateConnection->settings()->connectionType() == ConnectionSettings::Vpn)
    {
        // 移除后再加载进来以更新信息
        ui->connectionShowPage->removeConnectionFromList(path);
        ui->connectionShowPage->addConnection(updateConnection, "");
        if (ui->stackedWidget->currentIndex() != PAGE_SETTING)
            handleReturnPreviousPage();
    }
}
