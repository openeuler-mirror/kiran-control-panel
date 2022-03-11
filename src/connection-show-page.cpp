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

#include "connection-show-page.h"
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <QHBoxLayout>
#include <QLabel>
#include "ui_connection-show-page.h"

#define PROPERTY_CONNECTION_UUID "ConnectionUuid"
#define PROPERTY_ACTIVE_CONNECTION_PATH "ActiveConnectionPath"

ConnectionShowPage::ConnectionShowPage(QWidget* parent) : QWidget(parent), ui(new Ui::ConnectionShowPage)
{
    ui->setupUi(this);
    initUI();

    connect(ui->createConnectionButton, &QPushButton::clicked, [=]() {
        emit requestCreatConnection();
    });
}

ConnectionShowPage::~ConnectionShowPage()
{
    KLOG_DEBUG() << "ConnectionShowPage::~ConnectionShowPage()";
    delete ui;
}
void ConnectionShowPage::setTitle(QString title)
{
    ui->title->setText(title);
}

void ConnectionShowPage::initUI()
{
    m_switchButton = new KiranSwitchButton(this);
    ui->titleLayout->addWidget(m_switchButton);
}

void ConnectionShowPage::setSwitchButtonVisible(bool visible)
{
    m_switchButton->setVisible(visible);
}

void ConnectionShowPage::addConnectionToLists(Connection::Ptr ptr)
{
    //XXX:待优化
    QWidget* connectionShow = new QWidget();
    QLabel* connectionName = new QLabel(ptr->name(), connectionShow);
    QHBoxLayout* horizonLayout = new QHBoxLayout(connectionShow);
    QPushButton* editConnection = new QPushButton(connectionShow);
    editConnection->setProperty(PROPERTY_CONNECTION_UUID, ptr->uuid());

    horizonLayout->addWidget(connectionName);
    horizonLayout->addStretch();

    //TODO:插入已激活连接的标签
    QStringList activePaths = NetworkManager::activeConnectionsPaths();
    for (QString path : activePaths)
    {
        ActiveConnection::Ptr activeConnection = findActiveConnection(path);
        if (activeConnection->uuid() == ptr->uuid())
        {
            editConnection->setProperty(PROPERTY_ACTIVE_CONNECTION_PATH, activeConnection->path());
            QLabel* activatedLabel = new QLabel("Activated",connectionShow);
            horizonLayout->addWidget(activatedLabel);
        }
    }


    horizonLayout->addWidget(editConnection);
    connectionShow->setLayout(horizonLayout);

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(200, 50));

    //通过connectionLists->clear()释放所有的Item项
    //但插入Item中的Widget并不会一起释放，需要另外释放
    ui->connectionLists->addItem(item);
    ui->connectionLists->setItemWidget(item, connectionShow);
    connect(editConnection, &QPushButton::clicked, this, &ConnectionShowPage::handleEditButtonClicked);
}

void ConnectionShowPage::handleEditButtonClicked()
{
    QVariant UuidVar = sender()->property(PROPERTY_CONNECTION_UUID);
    QVariant pathVar = sender()->property(PROPERTY_ACTIVE_CONNECTION_PATH);

    QString Uuid = UuidVar.toString();
    QString activeConnectionPath = "";
    if(!pathVar.isNull())
    {
        activeConnectionPath =  pathVar.toString();
        KLOG_DEBUG() << "activeConnectionPath:"<< activeConnectionPath;
    }
    KLOG_DEBUG() << "Uuid:" << Uuid;
    emit requestEditConnection(Uuid,activeConnectionPath);
}

void ConnectionShowPage::clearConnectionLists()
{
    KLOG_DEBUG() << "clearConnectionLists()";
    int itemCount = ui->connectionLists->count();
    for (int i = 0; i < itemCount; ++i)
    {
        QListWidgetItem* item = ui->connectionLists->item(i);
        QWidget* itemWidget = ui->connectionLists->itemWidget(item);
        itemWidget->deleteLater();
    }

    ui->connectionLists->clear();
}
