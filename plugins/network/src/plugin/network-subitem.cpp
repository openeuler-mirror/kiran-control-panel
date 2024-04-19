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

#include "network-subitem.h"
#include "cpanel-network-widget.h"
#include <QEvent>
#include <qt5-log-i.h>
#include "utils.h"
#include "logging-category.h"

NetworkSubItem::NetworkSubItem(KiranControlPanel::PanelInterface* interface, QObject *parent)
    : m_interface(interface),
      QObject(parent)
{
}

NetworkSubItem::~NetworkSubItem()
{
}

bool NetworkSubItem::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_subitemWidget && event->type() == QEvent::Destroy)
    {
        m_subitemWidget = nullptr;
    }
    return QObject::eventFilter(watched, event);
}

QString NetworkSubItem::getID()
{
    return "Network";
}

QString NetworkSubItem::getName()
{
    return "";
}

QString NetworkSubItem::getCategory()
{
    return "network";
}

QString NetworkSubItem::getDesc()
{
    return "";
}

QString NetworkSubItem::getIcon()
{
    return "";
}

int NetworkSubItem::getWeight()
{
    return 0;
}

QWidget *NetworkSubItem::createWidget()
{
    m_subitemWidget = new CPanelNetworkWidget;
    m_subitemWidget->installEventFilter(this);
    connect(m_subitemWidget,&CPanelNetworkWidget::subItemsChanged,this,&NetworkSubItem::handleSubItemsChanged);
    return m_subitemWidget;
}

QVector<QPair<QString, QString> > NetworkSubItem::getSearchKeys()
{
    QVector<QPair<QString, QString>> searchKeys;
    m_subItemsList =  subItemsList();

    foreach(auto subItem,m_subItemsList)
    {
        KLOG_DEBUG(qLcNetwork) << "subItem:" << subItem;
        searchKeys.append({subItem,subItem});
    }

    return searchKeys;
}

bool NetworkSubItem::jumpToSearchEntry(const QString &key)
{
    if (!m_subitemWidget)
        return false;

    int index = m_subItemsList.indexOf(key);
    if(index != -1)
    {
        m_subitemWidget->setCurrentSubItem(index);
    }
    else
        return false;

    return true;
}

bool NetworkSubItem::haveUnSavedOptions()
{
    return false;
}

void NetworkSubItem::handleSubItemsChanged()
{
    m_interface->handlePluginSubItemInfoChanged(getID());
}

//TODO:
//1.用翻译文本做Key后续得改
//2.这些文本存在多次拷贝，后续改到一处，避免后续改动不全出问题
QStringList NetworkSubItem::subItemsList()
{
    QStringList subItemsList;
    auto wiredList = NetworkUtils::getManagedDeviceList(NetworkManager::Device::Ethernet);
    auto wirelessList = NetworkUtils::getManagedDeviceList(NetworkManager::Device::Wifi);

    if (!wiredList.isEmpty())
    {
        QString name = tr("Wired Network");
        subItemsList << name;
    }

    if( !wirelessList.isEmpty() )
    {
        QString name = tr("Wireless Network");
        subItemsList << name;
    }

    subItemsList << tr("VPN");
    subItemsList << tr("Network Details");

    return subItemsList;
}