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

#ifndef KIRAN_CPANEL_NETWORK_SETTINGPAGE_H
#define KIRAN_CPANEL_NETWORK_SETTINGPAGE_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>
#include <NetworkManagerQt/WiredSetting>
#include <QObject>
#include <QWidget>

class KiranTips;
class SettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingPage(QWidget *parent = nullptr);
    ~SettingPage();

    void initConnectionSettings(NetworkManager::ConnectionSettings::ConnectionType connectionType,
                                const QString &connectionUuid = "");

    void setConnection(const NetworkManager::Connection::Ptr &other);
    void setConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &other);
    void createConnectionSettings();

    virtual void initSettingPage();
    virtual void initSpecificSettings() = 0;
    virtual void saveSettingPage() = 0;
    virtual bool isInputValid();

    virtual void clearPtr();

public slots:
    void handleSaveButtonClicked(NetworkManager::ConnectionSettings::ConnectionType connectionType);

signals:
    void returnPreviousPage();
    void settingChanged();
    void settingUpdated();

protected:
    NetworkManager::Connection::Ptr m_connection = nullptr;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings = nullptr;
    NetworkManager::Ipv4Setting::Ptr m_ipv4Setting = nullptr;
    NetworkManager::Ipv6Setting::Ptr m_ipv6Setting = nullptr;
    NetworkManager::WiredSetting::Ptr m_wiredSetting = nullptr;
    NetworkManager::ConnectionSettings::ConnectionType m_connectionType;
    QString m_connectionUuid;
    QString m_activeConnectionPath = "";
    KiranTips *m_errorTip = nullptr;

private:
    bool m_isNewConnection;
};

#endif  // KIRAN_CPANEL_NETWORK_SETTINGPAGE_H
