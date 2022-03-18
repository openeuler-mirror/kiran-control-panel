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

ConnectionShowPage::ConnectionShowPage(QWidget* parent) : QWidget(parent), ui(new Ui::ConnectionShowPage)
{
    ui->setupUi(this);
    initUI();
    initConnect();
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

void ConnectionShowPage::initConnect()
{
    connect(ui->createConnectionButton, &QPushButton::clicked, [=]() { emit requestCreatConnection(); });
    connect(ui->connectionLists, &QListWidget::currentItemChanged, this, &ConnectionShowPage::handleSwitchConnection);
}

void ConnectionShowPage::handleSwitchConnection(QListWidgetItem* current, QListWidgetItem* previous)
{
    QWidget* widget = nullptr;
    ConnectionInfo connectionInfo;
    if (previous != nullptr)
    {
        //更新connection信息
        updateItemActivatedPath(previous);
        widget = ui->connectionLists->itemWidget(previous);
    }
    else
    {
        updateItemActivatedPath(m_activatedItem);
        widget = ui->connectionLists->itemWidget(m_activatedItem);
    }
    ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
    itemWidget->deactivateLabel();

    if (current != nullptr)
    {
        QString connectionPath = current->data(Qt::UserRole).value<ConnectionInfo>().connectionPath;
        KLOG_DEBUG() << "emit activateCurrentItemConnection(connectionPath)";
        emit activateCurrentItemConnection(connectionPath);
    }
}

void ConnectionShowPage::setSwitchButtonVisible(bool visible)
{
    m_switchButton->setVisible(visible);
}

void ConnectionShowPage::addConnectionToLists(Connection::Ptr ptr)
{
    ItemWidget* itemWidget = new ItemWidget();
    itemWidget->setName(ptr->name());

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(200, 50));

    ConnectionInfo connectionInfo;
    connectionInfo.uuid = ptr->uuid();
    connectionInfo.connectionPath = ptr->path();

    QStringList activePaths = NetworkManager::activeConnectionsPaths();
    for (QString path : activePaths)
    {
        ActiveConnection::Ptr activeConnection = findActiveConnection(path);
        if (activeConnection->uuid() == ptr->uuid())
        {
            connectionInfo.activeConnectionPath = activeConnection->path();
            itemWidget->activatedLabel();
            m_activatedItem = item;
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    //item中保存connection的相关信息
    item->setData(Qt::UserRole, var);

    ui->connectionLists->addItem(item);
    ui->connectionLists->setItemWidget(item, itemWidget);
    connect(itemWidget, &ItemWidget::editConnectionClicked, [=]() {
        QString uuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        KLOG_DEBUG() << "uuid:" << uuid;
        KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
        emit requestEditConnection(uuid, activeConnectionPath);
    });
}

/**
  通过connectionLists->clear()释放所有的Item项
  但插入Item中的Widget并不会一起释放，需要另外释放
*/
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
    m_activatedItem = nullptr;
}

void ConnectionShowPage::updateActivatedConnectionInfo(QString activatedPath)
{
    auto currentItem = ui->connectionLists->currentItem();
    QWidget* widget = ui->connectionLists->itemWidget(currentItem);
    ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
    itemWidget->activatedLabel();
    updateItemActivatedPath(currentItem, activatedPath);
}

void ConnectionShowPage::updateItemActivatedPath(QListWidgetItem* item, QString activatedPath)
{
    ConnectionInfo connectionInfo;
    connectionInfo = item->data(Qt::UserRole).value<ConnectionInfo>();
    connectionInfo.activeConnectionPath = activatedPath;
    QVariant var;
    var.setValue(connectionInfo);
    item->setData(Qt::UserRole, var);
}

//使用默认析够函数，父对象被释放时，会释放子对象
ItemWidget::ItemWidget(QWidget* parent) : QWidget(parent)
{
    m_connectionName = new QLabel(this);
    m_horizonLayout = new QHBoxLayout(this);
    m_editConnection = new QPushButton(this);
    m_activatedLabel = new QLabel("Activated", this);
    m_activatedLabel->setVisible(false);

    m_horizonLayout->addWidget(m_connectionName);
    m_horizonLayout->addStretch();
    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_editConnection);

    this->setLayout(m_horizonLayout);
    connect(m_editConnection, &QPushButton::clicked, this, &ItemWidget::editConnectionClicked);
}

void ItemWidget::setName(const QString& name)
{
    m_connectionName->setText(name);
}

void ItemWidget::activatedLabel()
{
    m_activatedLabel->setVisible(true);
}

void ItemWidget::deactivateLabel()
{
    m_activatedLabel->setVisible(false);
}
