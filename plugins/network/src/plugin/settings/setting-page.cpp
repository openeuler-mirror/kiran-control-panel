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

#include "setting-page.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/AdslSetting>
#include <NetworkManagerQt/Settings>

SettingPage::SettingPage(QWidget* parent) : QWidget(parent)
{
}

SettingPage::~SettingPage()
{
}

void SettingPage::initConnectionSettings(ConnectionSettings::ConnectionType connectionType, const QString& connectionUuid)
{
    m_connectionType = connectionType;
    m_connectionUuid = connectionUuid;

    if (m_connectionUuid.isEmpty())
    {
        KLOG_DEBUG() << "connection uuid is empty, creating new ConnectionSettings";
        createConnectionSettings();
        m_isNewConnection = true;
    }
    else
    {
        m_connection = findConnectionByUuid(m_connectionUuid);
        if (!m_connection)
        {
            KLOG_DEBUG() << "can't find connection by uuid";
        }
        m_connectionSettings = m_connection->settings();
        m_isNewConnection = false;
    }
}

void SettingPage::createConnectionSettings()
{
    m_connectionSettings = QSharedPointer<ConnectionSettings>(new NetworkManager::ConnectionSettings(m_connectionType));
    m_connectionUuid = m_connectionSettings->createNewUuid();
    m_connectionSettings->setUuid(m_connectionUuid);
    KLOG_DEBUG() << "create uuid:" << m_connectionSettings->uuid();
}

void SettingPage::clearPtr()
{
    m_connection.clear();
    m_connectionSettings.clear();
}

void SettingPage::setConnection(const Connection::Ptr& other)
{
    m_connection = other;
}

void SettingPage::setConnectionSettings(const ConnectionSettings::Ptr& other)
{
    m_connectionSettings = other;
}

void SettingPage::handleSaveButtonClicked(ConnectionSettings::ConnectionType connectionType)
{
    if(m_connectionSettings == nullptr)
    {
        initConnectionSettings(connectionType);
        initSettingPage();
        saveSettingPage();
        QDBusPendingReply<QDBusObjectPath> replyAdd = NetworkManager::addConnection(m_connectionSettings->toMap());
        replyAdd.waitForFinished();
        if (replyAdd.isError())
        {
            KLOG_DEBUG() << "add connection failed," << replyAdd.error();
        }
        else
            KLOG_DEBUG() << "add new connection";
    }
    else
    {
        saveSettingPage();
        //只有无线网络使用自定义settingUpdated信号，因为未连接无线网络前不存在本地Setting，无法在初始化时监听信号
        connect(m_connection.data(),&NetworkManager::Connection::updated,this,&SettingPage::settingUpdated,Qt::UniqueConnection);
        QDBusPendingReply<> replyUpdate = m_connection->update(m_connectionSettings->toMap());
        replyUpdate.waitForFinished();
        if (replyUpdate.isError())
        {
            KLOG_DEBUG() << "error occurred while updating the connection" << replyUpdate.error();
        }
    }
}

void SettingPage::initSettingPage()
{
    initSpecificSettings();
    initWidgets();
}

bool SettingPage::isInputValid()
{
    return false;
}
