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
#include "wired-page.h"
#include "ui_wired-page.h"

#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>

enum WiredEditPages
{
    PAGE_WIRED,
    PAGE_WIRED_SETTING
};

WiredPage::WiredPage(QWidget *parent) : QWidget(parent), ui(new Ui::WiredPage)
{
    ui->setupUi(this);
    initUI();
    initConnecton();
}

WiredPage::~WiredPage()
{
    delete ui;
}

void WiredPage::initUI()
{
    ui->connectionShowPage->setTitle(tr("Wired Network Adapter"));
    ui->connectionShowPage->setSwitchButtonVisible(true);
    showWiredConnections();
}

void WiredPage::showWiredConnections()
{
    const Connection::List connectionList = NetworkManager::listConnections();
    for (Connection::Ptr conn : connectionList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
        {
            KLOG_DEBUG() << "conn->uuid():" << conn->uuid();
            KLOG_DEBUG() << "conn->name():" << conn->name();
            KLOG_DEBUG() << "conn->path():" << conn->path();
            KLOG_DEBUG() << "----------------";
            ui->connectionShowPage->addConnectionToLists(conn);
        }
    }

//    QStringList activePaths = NetworkManager::activeConnectionsPaths();
//    for (QString path : activePaths)
//    {
//        ActiveConnection::Ptr activeConnection = findActiveConnection(path);
//        KLOG_DEBUG() << "activeConnection->state(): " << activeConnection->state();
//        KLOG_DEBUG() << "activeConnection->type(): " << activeConnection->type();
//        KLOG_DEBUG() << "activeConnection->path(): " << activeConnection->path();
//        KLOG_DEBUG() << "activeConnection->id(): " << activeConnection->id();
//        KLOG_DEBUG() << "activeConnection->uuid(): " << activeConnection->uuid();
//        if (activeConnection->type() == ConnectionSettings::ConnectionType::Wired)
//        {
//            m_activeConnectionUuid = activeConnection->uuid();
//            m_activeConnectionPath = activeConnection->path();
//            m_activeConnection = activeConnection;
//        }
//    }



}

void WiredPage::initConnecton()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED_SETTING);
        ui->wiredSettingPage->refreshSettingPage();
    });

    connect(ui->connectionShowPage, &ConnectionShowPage::requestEditConnection, [=](QString uuid,QString activeConnectionPath) {
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED_SETTING);
        ui->wiredSettingPage->initConnectionSettings(ConnectionSettings::ConnectionType::Wired, uuid);
        ui->wiredSettingPage->refreshSettingPage(activeConnectionPath);
    });

    connect(ui->wiredSettingPage, &WiredSettingPage::returnPreviousPage, [=]() {
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED);
        ui->wiredSettingPage->clearPtr();
    });

    connect(ui->wiredSettingPage, &WiredSettingPage::settingChanged, [=]() {
        ui->wiredSettingPage->clearPtr();
        ui->connectionShowPage->clearConnectionLists();
        //保存设置生效后，刷新列表时，NetworkManager::listConnections()接口返回的数据还未更新，故延时等待
        //测试等待20msec后能够得到更新的数据
        QTimer::singleShot(30, this, SLOT(showWiredConnections()));
        ui->stackedWidget->setCurrentIndex(PAGE_WIRED);
    });

    connect(settingsNotifier(),&SettingsNotifier::connectionAdded,[=](const QString &path){
        KLOG_DEBUG() << "connectionAdded path:" << path;
        //并不知道增加的连接类型，需要另外判断
//        QSharedPointer<Connection>
//        findConnection(path);

    });


//    SettingsNotifier::connectionAddComplete()
}
