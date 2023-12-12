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

#include "setting-page.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/AdslSetting>
#include <NetworkManagerQt/Settings>
#include "logging-category.h"
using namespace NetworkManager;

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
        KLOG_DEBUG(qLcNetwork) << "connection uuid is empty, creating new ConnectionSettings";
        createConnectionSettings();
        m_isNewConnection = true;
    }
    else
    {
        m_connection = findConnectionByUuid(m_connectionUuid);
        if (!m_connection)
        {
            KLOG_DEBUG(qLcNetwork) << "can't find connection by uuid";
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
    KLOG_DEBUG(qLcNetwork) << "create uuid:" << m_connectionSettings->uuid();
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
    if (m_connectionSettings == nullptr)
    {
        initConnectionSettings(connectionType);
        initSettingPage();
        saveSettingPage();

        QDBusPendingReply<QDBusObjectPath> replyAdd = NetworkManager::addConnection(m_connectionSettings->toMap());
        replyAdd.waitForFinished();
        if (replyAdd.isError())
        {
            KLOG_DEBUG(qLcNetwork) << "add connection failed," << replyAdd.error();
        }
        else
        {
            KLOG_DEBUG(qLcNetwork) << "add new connection reply:" << replyAdd.reply();
        }
    }
    else
    {
        saveSettingPage();
        // 只有无线网络使用自定义settingUpdated信号，因为未连接无线网络前不存在本地Setting，无法在初始化时监听信号
        connect(m_connection.data(), &NetworkManager::Connection::updated, this, &SettingPage::settingUpdated, Qt::UniqueConnection);
        /**
         * NOTE:
         * 这里是为了修复控制中心网络设置克隆MAC地址设置保存之后,再修改清空，清空克隆MAC地址不生效的问题，同时兼容低版本的kf5-networkmanager-qt5
         * 问题原因：高版本的NetworkManager成功设置克隆MAC地址后会有，两个属性：assigned-mac-addres (新属性)，cloned-mac-address(即将被废弃)
         * 两个属性作用是一样的，低版本的kf5-networkmanager-qt5只有对cloned-mac-address(即将被废弃)进行设置的接口，
         * 无法对assigned-mac-addres(新属性)进行操作。需要将这个两个属性都清空才能清空克隆MAC地址
         * 因此这里手动将assigned-mac-addres相关的设置进行清空。
        */
        NMVariantMapMap settingsMap = m_connectionSettings->toMap();
        if (settingsMap.contains("802-3-ethernet"))
        {
            QVariantMap varMap = settingsMap.value("802-3-ethernet");
            if (!varMap.contains("cloned-mac-address") && varMap.contains("assigned-mac-address"))
            {
                varMap.remove("assigned-mac-address");
                settingsMap.insert("802-3-ethernet", varMap);
            }
        }

        QDBusPendingReply<> replyUpdate = m_connection->update(settingsMap);
        replyUpdate.waitForFinished();
        if (replyUpdate.isError())
        {
            KLOG_DEBUG(qLcNetwork) << "error occurred while updating the connection" << replyUpdate.error();
        }
    }
}

void SettingPage::initSettingPage()
{
    initSpecificSettings();
}

bool SettingPage::isInputValid()
{
    return false;
}
