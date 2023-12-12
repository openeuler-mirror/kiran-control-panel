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

#include "disconnect-and-delete-button.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Manager>
#include <QTimer>
#include "status-notification.h"
#include "ui_disconnect-and-delete-button.h"
#include "logging-category.h"
using namespace NetworkManager;

DisconnectAndDeleteButton::DisconnectAndDeleteButton(QWidget *parent) : QWidget(parent), ui(new Ui::DisconnectAndDeleteButton)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

DisconnectAndDeleteButton::~DisconnectAndDeleteButton()
{
    delete ui;
}

void DisconnectAndDeleteButton::initUI()
{
    ui->ignoreButton->setVisible(false);
}

void DisconnectAndDeleteButton::initButton(SettingConnectionStatus connectionStatus, const QString &activeConnectionPath)
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
    m_activeConnectionPath = activeConnectionPath;

    auto activeConnectionPtr = NetworkManager::findActiveConnection(m_activeConnectionPath);
    if(activeConnectionPtr.isNull())
    {
       return;
    }
    ConnectionSettings::ConnectionType connectionType = activeConnectionPtr->connection()->settings()->connectionType();
    if (connectionType == ConnectionSettings::Wireless)
    {
        ui->deleteButton->setVisible(false);
        ui->ignoreButton->setVisible(true);
    }
}

void DisconnectAndDeleteButton::initConnection()
{
    connect(ui->disconnectButton, &QPushButton::clicked, [=]()
            {
        QDBusPendingReply<> reply = NetworkManager::deactivateConnection(m_activeConnectionPath);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_INFO(qLcNetwork) << "Disconnect failed:" << reply.error();
        }
        else
            KLOG_DEBUG(qLcNetwork) << "deactivateConnection reply:" << reply.reply();
        emit disconnectButtonClicked(); });
    connect(ui->deleteButton, &QPushButton::clicked, this, &DisconnectAndDeleteButton::handleDeleteConnection);
    connect(ui->ignoreButton, &QPushButton::clicked, this, &DisconnectAndDeleteButton::handleIgnoreWireless);
}

void DisconnectAndDeleteButton::setConnectionPtr(const Connection::Ptr &connection)
{
    m_connection = connection;
}

void DisconnectAndDeleteButton::handleDeleteConnection()
{
    QString tip = QString(tr("Are you sure you want to delete the connection %1")).arg(m_connection->name());
    KiranMessageBox::KiranStandardButton btn = KiranMessageBox::message(this, tr("Warning"),
                                                                        tip,
                                                                        KiranMessageBox::Yes | KiranMessageBox::No);
    if (btn == KiranMessageBox::Yes)
    {
        QString connectionName = m_connection->name();
        QDBusPendingReply<> reply = m_connection->remove();
        reply.waitForFinished();
        StatusNotification::connectionDeleteNotify(connectionName);
        if (reply.isError())
        {
            KLOG_INFO(qLcNetwork) << "Delete the connection failed:" << reply.error();
        }
        emit deleteButtonClicked();
    }
}

void DisconnectAndDeleteButton::clearPtr()
{
    m_connection.clear();
}

// Note:ignore无线网络先断开网络再删除配置，功能与delete部分重合
void DisconnectAndDeleteButton::handleIgnoreWireless()
{
    QDBusPendingReply<> reply = NetworkManager::deactivateConnection(m_activeConnectionPath);
    reply.waitForFinished();
    if (reply.isError())
        KLOG_DEBUG(qLcNetwork) << "Disconnect failed:" << reply.error();

    /*
     * Note:deactivate后，通过信号发出deactivate的状态通知，通知需要从connection中获取id信息
     * 如果deactivate后立马删除connection,可能导致状态通知获取不到相应的信息
     * 故延时一段时间，以便状态通知能获取到id信息
     * 有待改进
     * */
    QTimer::singleShot(100, this, [this]()
                       {
                        QDBusPendingReply<> reply = m_connection->remove();
                        reply.waitForFinished();
                        emit deleteButtonClicked(); });
}