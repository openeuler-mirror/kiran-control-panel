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
#include "wired-manager.h"
#include <libnm/nm-connection.h>
#include <qt5-log-i.h>
#include <style-property.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QPointer>
#include <QScrollBar>
#include "signal-forward.h"
#include "status-notification.h"
#include "ui_wired-manager.h"
using namespace NetworkManager;

WiredManager::WiredManager(const QString &devicePath, QWidget *parent) : Manager(parent), ui(new Ui::WiredManager)
{
    ui->setupUi(this);
    m_devicePath = devicePath;
    initUI();
    initConnection();
}

WiredManager::~WiredManager()
{
    ui->wiredSettingPage->clearPtr();
    delete ui;
}

void WiredManager::initUI()
{
    ui->connectionShowPage->init(ConnectionSettings::Wired, m_devicePath);
    ui->connectionShowPage->setTitle(tr("Wired Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(false);
    Kiran::StylePropertyHelper::setButtonType(ui->saveButton, Kiran::BUTTON_Default);
}

void WiredManager::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::creatConnection, this, &WiredManager::handleCreatConnection);
    connect(ui->connectionShowPage, &ConnectionShowPage::editConnection, this, &WiredManager::handleEditConnection);

    connect(ui->returnButton, &QPushButton::clicked, this, &WiredManager::handleReturnPreviousPage);
    connect(ui->saveButton, &QPushButton::clicked, this, &WiredManager::handleSaveButtonClicked);

    connect(ui->wiredSettingPage, &WiredSettingPage::returnPreviousPage, this, &WiredManager::handleReturnPreviousPage);

    connect(ui->connectionShowPage, &ConnectionShowPage::connectionUpdated, this, &WiredManager::handleConnectionUpdated);
    connect(ui->connectionShowPage, &ConnectionShowPage::activateSelectedConnection, this, &WiredManager::handleActivateSelectedConnection);

    connect(SignalForward::instance(), &SignalForward::wiredConnectionAdded, this, &WiredManager::handleNotifierConnectionAdded);
    connect(SignalForward::instance(), &SignalForward::wiredActiveConnectionAdded, this, &WiredManager::handleActiveConnectionAdded);

    connect(SignalForward::instance(), &SignalForward::connectionRemoved, this, &WiredManager::handleNotifierConnectionRemoved);
    connect(SignalForward::instance(), &SignalForward::activeConnectionRemoved, this, &WiredManager::handleActiveConnectionRemoved);
}

void WiredManager::handleCreatConnection()
{
    ui->wiredSettingPage->showSettingPage();
    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void WiredManager::handleEditConnection(const QString &uuid, QString activeConnectionPath)
{
    ui->wiredSettingPage->initConnectionSettings(ConnectionSettings::ConnectionType::Wired, uuid);
    ui->wiredSettingPage->initSettingPage();
    ui->wiredSettingPage->showSettingPage(activeConnectionPath);
    QPointer<QScrollBar> scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(0);
    ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
}

void WiredManager::handleActivateSelectedConnection(const QString &connectionPath, const QString &connectionParameter)
{
    Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);

    auto devicestate = device->state();
    KLOG_DEBUG() << "device state:" << devicestate ;
    if(devicestate != Device::Unavailable)
    {
        QDBusPendingReply<QDBusObjectPath> reply =
            NetworkManager::activateConnection(connectionPath, m_devicePath, connectionParameter);

        reply.waitForFinished();
        if (reply.isError())
        {
            // 此处处理进入激活流程失败的原因，并不涉及流程中某个具体阶段失败的原因
            KLOG_ERROR() << "activate connection failed:" << reply.error();
            QString errorMessage = reply.error().message();
            if (errorMessage.contains("device has no carrier"))
                StatusNotification::connectitonFailedNotifyByReason(tr("The carrier is pulled out"));
            else
                StatusNotification::connectitonFailedNotify(connectionPath);
        }
        else
            KLOG_DEBUG() << "activateConnection reply:" << reply.reply();
    }
    else
        StatusNotification::connectitonFailedNotifyByReason(tr("The current device is not available"));
}

// 获取到当前激活对象后，开启等待动画，判断完激活状态后停止等待动画
void WiredManager::handleActiveConnectionAdded(const QString &path)
{
    ActiveConnection::Ptr activatedConnection = findActiveConnection(path);
    if (activatedConnection.isNull())
        return;
    QStringList deviceList = activatedConnection->devices();
    if (deviceList.contains(m_devicePath))
    {
        QString uuid = activatedConnection->uuid();
        auto *activeItemWidget = ui->connectionShowPage->findItemWidgetByUuid(uuid);
        if (activeItemWidget != nullptr)
        {
            ui->connectionShowPage->updateItemWidgetActivePath(activeItemWidget, path);
            KLOG_DEBUG() << "activatedConnection->state():" << activatedConnection->state();
            switch (activatedConnection->state())
            {
            case ActiveConnection::State::Activating:
                handleStateActivating(path);
                break;
            case ActiveConnection::State::Activated:
                handleStateActivated(path);
                break;
            default:
                break;
            }
        }
        connect(activatedConnection.data(), &ActiveConnection::stateChanged, this, &WiredManager::handleActiveConnectionStateChanged);
    }
}

void WiredManager::handleStateActivating(const QString &activePath)
{
          // 加载等待动画
    ui->connectionShowPage->setItemWidgetStatus(activePath, ActiveConnection::State::Activating);
}

void WiredManager::handleActiveConnectionRemoved(const QString &path)
{
    ui->connectionShowPage->handleActiveStateDeactivated(path);
}

// TODO:提升代码，增强复用性
void WiredManager::handleStateActivated(const QString &activePath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activePath);
    if (activeConnection.isNull())
    {
        return;
    }
    QStringList deviceList = activeConnection->devices();
    if (deviceList.contains(m_devicePath) && (activeConnection->type() == ConnectionSettings::Wired))
    {
        ui->connectionShowPage->setItemWidgetStatus(activePath, ActiveConnection::State::Activated);
        ui->connectionShowPage->sort();
    }
}

void WiredManager::handleStateDeactivated(const QString &deactivatedPath)
{
    ui->connectionShowPage->handleActiveStateDeactivated(deactivatedPath);
}

void WiredManager::handleReturnPreviousPage()
{
    ui->wiredSettingPage->clearPtr();
    ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
}

void WiredManager::handleNotifierConnectionAdded(const QString &path)
{
    KLOG_DEBUG() << "Connection Added :" << path;
    Connection::Ptr connection = findConnection(path);
    ui->connectionShowPage->addConnection(connection, m_devicePath);
}

// Note:当connection被移除时，由于连接可能已经被删除，所有并不能通过findConnection(path)找到该连接对象，进而知道连接类型
void WiredManager::handleNotifierConnectionRemoved(const QString &path)
{
    KLOG_DEBUG() << "Connection Removed :" << path;
    ui->connectionShowPage->removeConnectionFromList(path);
}

void WiredManager::handleSaveButtonClicked()
{
    if (ui->wiredSettingPage->isInputValid())
    {
        ui->wiredSettingPage->handleSaveButtonClicked(ConnectionSettings::ConnectionType::Wired);
        handleReturnPreviousPage();
    }
    else
    {
        KLOG_DEBUG() << "Invalid input exists";
    }
}

void WiredManager::handleConnectionUpdated(const QString &path)
{
    KLOG_DEBUG() << "Connection updated:" << path;
    Connection::Ptr updateConnection = findConnection(path);
    if (updateConnection->settings()->connectionType() == ConnectionSettings::Wired)
    {
        //移除后再加载进来以更新信息
        ui->connectionShowPage->removeConnectionFromList(path);
        ui->connectionShowPage->addConnection(updateConnection, "");
        if (ui->stackedWidget->currentIndex() != PAGE_SETTING)
            handleReturnPreviousPage();

        QString updateConnectionPath = updateConnection->path();
        ActiveConnection::List activeConnectionLists = activeConnections();
        //已连接的网络的配置被修改后，点击保存 ，应该重新连接网络，以使配置生效
        for (auto activeConn : activeConnectionLists)
        {
            if (activeConn->connection()->path() == updateConnectionPath)
            {
                auto deviceLists = activeConn->devices();
                if (deviceLists.contains(m_devicePath))
                {
                    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(activeConn->connection()->path());
                    handleActivateSelectedConnection(updateConnectionPath, "");
                }
            }
        }
    }
}
