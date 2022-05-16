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

#include "manager-tray.h"
#include "wired-tray-page.h"
#include "wireless-tray-page.h"
#include "status-notifier-manager.h"
#include <qt5-log-i.h>
#include <QVBoxLayout>
#define STATUS_NOTIFIER_MANAGER "org.kde.StatusNotifierManager"
#define STATUS_NOTIFIER_MANAGER_OBJECT_NAME "/StatusNotifierManager"
ManagerTray::ManagerTray(QWidget *parent) : QWidget(parent)
{
    init();
}

ManagerTray::~ManagerTray()
{
}

void ManagerTray::init()
{
    initTrayIcon();
    initWiredTrayPage();
    m_statusNotifierManager = new StatusNotifierManagerInterface(STATUS_NOTIFIER_MANAGER,STATUS_NOTIFIER_MANAGER_OBJECT_NAME,QDBusConnection::sessionBus(),this);
    initUI();
    initConnect();
}

void ManagerTray::initUI()
{
    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->addWidget(m_wiredTrayPage);
//    m_verticalLayout->addStretch();
    m_verticalLayout->setMargin(0);
//    this->setLayout(m_verticalLayout);
    this->setWindowFlags(Qt::Popup | Qt::BypassWindowManagerHint);
}

void ManagerTray::initConnect()
{
    connect(m_systemTray, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason){
        handleTrayClicked(reason);
    });
}

void ManagerTray::initTrayIcon()
{
    m_systemTray = new QSystemTrayIcon();
    m_systemTray->icon();
    m_systemTray->setIcon(QIcon(":/kcp-network-images/wired-connection.svg"));
    m_systemTray->show();
}

void ManagerTray::initMenu()
{
}

void ManagerTray::initWiredTrayPage()
{
    m_wiredTrayPage = new WiredTrayPage(this);
}

void ManagerTray::initWirelessTrayPage()
{

}


void ManagerTray::handleTrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        showTrayPage();
        break;
    default:
        break;
    }
}

void ManagerTray::showTrayPage()
{
    KLOG_DEBUG() << "showTrayPage";
    setTrayPagePos();
    this->show();
}

void ManagerTray::setTrayPagePos()
{
    getTrayGeometry();
    int pageWidth = 240;
    //TODO:随插入项高度动态变化
    int maxHeight = 434;
    int pageHeight = this->sizeHint().height();
    this->setGeometry(m_xTray - pageWidth / 2, m_yTray - pageHeight, pageWidth, pageHeight);
}

void ManagerTray::getTrayGeometry()
{
    //名称待修改
    QDBusPendingReply<QString> getGeometry = m_statusNotifierManager->GetGeometry("kiran-cpanel-network");
    KLOG_DEBUG() << "getGeometry.value():" << getGeometry.value();

    double height, width, x, y;
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getGeometry.value().toLatin1(), &jsonParseError);
    if (!doc.isNull() && jsonParseError.error == QJsonParseError::NoError)
    {
        if (doc.isObject() && jsonParseError.error == QJsonParseError::NoError)
        {
            if (doc.isObject())
            {
                QJsonObject object = doc.object();
                QStringList list = object.keys();
                height = object.value("height").toDouble();
                width = object.value("width").toDouble();
                x = object.value("x").toDouble();
                y = object.value("y").toDouble();
            }
        }
    }
    m_heightTray = static_cast<int>(height);
    m_widthTray = static_cast<int>(width);
    m_xTray = static_cast<int>(x);
    m_yTray = static_cast<int>(y);
}

void ManagerTray::setTrayIcon()
{

}
