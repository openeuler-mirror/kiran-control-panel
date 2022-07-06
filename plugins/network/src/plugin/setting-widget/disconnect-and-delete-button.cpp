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

#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include "status-notification.h"
#include "disconnect-and-delete-button.h"
#include "ui_disconnect-and-delete-button.h"

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
}

void DisconnectAndDeleteButton::initConnection()
{
    connect(ui->disconnectButton, &QPushButton::clicked, [=]() {
        QDBusPendingReply<> reply = NetworkManager::deactivateConnection(m_activeConnectionPath);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_INFO() << "Disconnect failed:" << reply.error();
        }
        emit disconnectButtonClicked();
    });
    connect(ui->deleteButton, &QPushButton::clicked, this, &DisconnectAndDeleteButton::handleDeleteConnection);
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
            KLOG_INFO() << "Delete the connection failed:" << reply.error();
        }
        emit deleteButtonClicked();
    }
}

void DisconnectAndDeleteButton::clearPtr()
{
    m_connection.clear();
}
