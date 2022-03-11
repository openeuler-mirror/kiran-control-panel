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

#ifndef KIRAN_CPANEL_NETWORK_SETTINGPAGE_H
#define KIRAN_CPANEL_NETWORK_SETTINGPAGE_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <QObject>
#include <QWidget>

using namespace NetworkManager;

class SettingPage : public QWidget
{
    Q_OBJECT
public:
    SettingPage(QWidget *parent = nullptr);
    ~SettingPage();

    void init();
    void initConnectionSettings(ConnectionSettings::ConnectionType connectionType,
                                                              const QString& connectionUuid = "");

    void createConnectionSettings();
    void clearPtr();

    int connectionSuffixNum(QString &connName,ConnectionSettings::ConnectionType connType);
signals:
    void returnPreviousPage();
    void settingChanged();

protected:
    NetworkManager::Connection::Ptr m_connection = nullptr;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings = nullptr;
    NetworkManager::Ipv4Setting::Ptr m_ipv4Setting = nullptr;
    QString m_connectionUuid;
    QString m_activeConnectionPath;

private:
    NetworkManager::ConnectionSettings::ConnectionType m_connectionType;

    bool m_isNewConnection;
};

#endif  //KIRAN_CPANEL_NETWORK_SETTINGPAGE_H
