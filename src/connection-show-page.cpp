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
#include <NetworkManagerQt/Settings>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QSvgRenderer>
#include "animation-loading-label.h"
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
    connect(ui->connectionLists, &QListWidget::itemClicked, this, &ConnectionShowPage::handleConnectionItemClicked);
}

void ConnectionShowPage::handleConnectionItemClicked(QListWidgetItem* item)
{
    //判断是否已激活
    if (item != m_previousActivatedItem)
    {
        QString connectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().connectionPath;
        KLOG_DEBUG() << "emit activateCurrentItemConnection(connectionPath)";
        emit requestActivateCurrentItemConnection(connectionPath);
    }
    else
        KLOG_DEBUG() << "this connection is activated";
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

            connect(activeConnection.data(), &ActiveConnection::stateChanged, [=](ActiveConnection::State state) {
                switch (state)
                {
                case ActiveConnection::State::Deactivated:
                    KLOG_DEBUG() << "ActiveConnection::State::Deactivated";
                    //更新item所带信息，清空已激活路径
                    KLOG_DEBUG() << "Deactivated: activeConnection->path() : " << activeConnection->path();
                    emit deactivatedItemConnection(activeConnection->path());
                    break;
                default:
                    break;
                }
            });

            itemWidget->activatedLabel();
            m_previousActivatedItem = item;
        }
    }

    QVariant var;
    var.setValue(connectionInfo);
    //item中保存connection的相关信息
    item->setData(Qt::UserRole, var);
    ui->connectionLists->addItem(item);
    ui->connectionLists->setItemWidget(item, itemWidget);

    ui->connectionLists->setMaximumHeight(ui->connectionLists->sizeHintForRow(0) * ui->connectionLists->count() + (2 * ui->connectionLists->frameWidth()));
    //    KLOG_DEBUG() << "ui->connectionLists->maximumHeight():" << ui->connectionLists->maximumHeight();
    connect(itemWidget, &ItemWidget::editConnectionClicked, [=]() {
        QString uuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        QString activeConnectionPath = item->data(Qt::UserRole).value<ConnectionInfo>().activeConnectionPath;
        KLOG_DEBUG() << "uuid:" << uuid;
        KLOG_DEBUG() << "activeConnectionPath:" << activeConnectionPath;
        emit requestEditConnection(uuid, activeConnectionPath);
    });
}

void ConnectionShowPage::removeConnectionFromLists(const QString& path)
{
    for (int i = 0; i < ui->connectionLists->count(); ++i)
    {
        QListWidgetItem* item = ui->connectionLists->item(i);
        QString itemPath = item->data(Qt::UserRole).value<ConnectionInfo>().connectionPath;
        if (path == itemPath)
        {
            ui->connectionLists->takeItem(i);
            //使用takeItem:Items removed from a list widget will not be managed by Qt, and will need to be deleted manually.
            QWidget* itemWidget = ui->connectionLists->itemWidget(item);
            itemWidget->deleteLater();
            delete item;
            break;
        }
    }
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
    m_previousActivatedItem = nullptr;
}

void ConnectionShowPage::findItemByUuid(const QString& uuid)
{
    for (int i = 0; i < ui->connectionLists->count(); ++i)
    {
        QListWidgetItem* item = ui->connectionLists->item(i);
        QString itemUuid = item->data(Qt::UserRole).value<ConnectionInfo>().uuid;
        if (uuid == itemUuid)
        {
            m_currentActiveItem = item;
            break;
        }
    }
}

//Note:不应该通过点击item来加载动画，而应该通过信号来激活加载动画
void ConnectionShowPage::connectionItemLoadingAnimation()
{
    QWidget* widget = ui->connectionLists->itemWidget(m_currentActiveItem);
    ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
    itemWidget->setLoadingStatus(true);
    itemWidget->setLabelVisible(true);
}

void ConnectionShowPage::updateActivatedConnectionInfo(QString activatedPath)
{
    m_previousActivatedItem = m_currentActiveItem;
    QWidget* widget = ui->connectionLists->itemWidget(m_currentActiveItem);
    ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
    itemWidget->setLoadingStatus(false);
    itemWidget->activatedLabel();

    updateItemActivatedPath(m_currentActiveItem, activatedPath);
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

void ConnectionShowPage::clearDeactivatedConnectionInfo(const QString& deactivatedPath)
{
    if (m_previousActivatedItem != nullptr)
    {
        QWidget* widget = ui->connectionLists->itemWidget(m_previousActivatedItem);
        ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
        itemWidget->deactivateLabel();
        KLOG_DEBUG() << "itemWidget->deactivateLabel()";
        //清空已激活路径,更新item所带信息
        updateItemActivatedPath(m_previousActivatedItem);
        m_previousActivatedItem = nullptr;
    }
    else
    {
        //对应网络激活过程中激活失败，直接断开的情况
        QWidget* widget = ui->connectionLists->itemWidget(m_currentActiveItem);
        ItemWidget* itemWidget = qobject_cast<ItemWidget*>(widget);
        itemWidget->deactivateLabel();
    }
}

//使用默认析够函数，父对象被释放时，会释放子对象
ItemWidget::ItemWidget(QWidget* parent) : QWidget(parent)
{
    m_connectionName = new QLabel(this);
    m_horizonLayout = new QHBoxLayout(this);
    m_editConnection = new QPushButton(this);
    m_activatedLabel = new AnimationLoadingLabel(this);

    m_activatedLabel->setVisible(false);

    m_horizonLayout->addWidget(m_connectionName);
    m_horizonLayout->addStretch();
    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_editConnection);
    m_horizonLayout->setMargin(0);

    this->setLayout(m_horizonLayout);
    connect(m_editConnection, &QPushButton::clicked, this, &ItemWidget::editConnectionClicked);
}

void ItemWidget::setName(const QString& name)
{
    m_connectionName->setText(name);
}

void ItemWidget::activatedLabel()
{
    QSvgRenderer svgRenderer(QString("/home/luoqing/git/icon_correct.svg"), this);
    QPixmap pixmap(QSize(16, 16));
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    svgRenderer.render(&painter);
    m_activatedLabel->setPixmap(pixmap);
    m_activatedLabel->setAlignment(Qt::AlignCenter);
    m_activatedLabel->setVisible(true);
}

void ItemWidget::deactivateLabel()
{
    m_activatedLabel->setVisible(false);
}

void ItemWidget::setLoadingStatus(bool isLoading)
{
    m_activatedLabel->setLoadingStatus(isLoading);
}

void ItemWidget::setLabelVisible(bool isVisible)
{
    m_activatedLabel->setVisible(isVisible);
}
