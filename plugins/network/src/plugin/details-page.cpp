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

#include "details-page.h"
#include <qt5-log-i.h>
#include <QTimer>
#include "connection-details-widget.h"
#include "ui_details-page.h"
using namespace NetworkManager;

DetailsPage::DetailsPage(QWidget *parent) : QWidget(parent), ui(new Ui::DetailsPage)
{
    ui->setupUi(this);
    initUI();
    initConnect();
}

DetailsPage::~DetailsPage()
{
    delete ui;
}

void DetailsPage::initUI()
{
    ui->selectConnectionwidget->setVisible(false);
    Device::List deviceList = networkInterfaces();
    for (Device::Ptr device : deviceList)
    {
        if ((device->type() == Device::Wifi) ||
            (device->type() == Device::Ethernet))
        {
            ActiveConnection::Ptr activeConnection = device->activeConnection();
            if (activeConnection != nullptr)
            {
                if (activeConnection->state() == ActiveConnection::Activated)
                {
                    m_deviceList << device;
                }
            }
        }
    }

    if (m_deviceList.count() > 1)
        initMultiConnectionDetailsWidget();
    else if (m_deviceList.count() == 1)
    {
        QWidget *widget = new ConnectionDetailsWidget(m_deviceList.at(0), this);
        ui->stackedWidget->addWidget(widget);
    }
    else if (m_deviceList.count() == 0)
    {
        QWidget *widget = new ConnectionDetailsWidget(nullptr, this);
        ui->stackedWidget->addWidget(widget);
    }
}

void DetailsPage::initMultiConnectionDetailsWidget()
{
    ui->selectConnectionwidget->setVisible(true);
    for (Device::Ptr device : m_deviceList)
    {
        ActiveConnection::Ptr activeConnection = device->activeConnection();
        ui->activatedConnectionComboBox->addItem(activeConnection->id());
        QWidget *widget = new ConnectionDetailsWidget(device, this);
        ui->stackedWidget->addWidget(widget);
    }
    connect(ui->activatedConnectionComboBox, QOverload<int>::of(&QComboBox::activated), this,
            &DetailsPage::handleActivatedConnectionComboBoxActivated, Qt::UniqueConnection);
}

void DetailsPage::handleActivatedConnectionComboBoxActivated(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void DetailsPage::initConnect()
{
    connect(notifier(), &Notifier::deviceAdded, this, &DetailsPage::handleDeviceAdded);
    connect(notifier(), &Notifier::deviceRemoved, this, &DetailsPage::handleDeviceRemoved);
    connect(notifier(), &Notifier::activeConnectionAdded, this, &DetailsPage::handleActiveConnectionAdded);
    connect(notifier(), &Notifier::activeConnectionRemoved, this, &DetailsPage::handleActiveConnectionRemoved);
    connect(notifier(), &Notifier::activeConnectionsChanged, this, &DetailsPage::handleActiveConnectionChanged);
}

void DetailsPage::handleDeviceAdded(const QString &devicePath)
{
    reload();
}

void DetailsPage::handleDeviceRemoved(const QString &devicePath)
{
    reload();
}

void DetailsPage::handleActiveConnectionChanged()
{
    reload();
}

void DetailsPage::handleActiveConnectionAdded(const QString &activeConnectionPath)
{
    ActiveConnection::Ptr activeConnection = findActiveConnection(activeConnectionPath);
    connect(activeConnection.data(), &ActiveConnection::stateChanged, this, &DetailsPage::handleActiveConnectionStateChanged, Qt::UniqueConnection);
}

void DetailsPage::handleActiveConnectionRemoved(const QString &activeConnectionPath)
{
    reload();
}

void DetailsPage::handleActiveConnectionStateChanged(ActiveConnection::State state)
{
    if (state == ActiveConnection::Activated)
    {
        reload();
    }
}

void DetailsPage::clear()
{
    m_deviceList.clear();
    ui->activatedConnectionComboBox->clear();
    int count = ui->stackedWidget->count();
    for (int i = 0; i < count; i++)
    {
        QWidget *widget = ui->stackedWidget->currentWidget();
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }
}

void DetailsPage::reload()
{
    QTimer::singleShot(1000, this, [=]()
                       {
                           clear();
                           initUI(); });
}
