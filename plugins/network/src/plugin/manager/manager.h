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

#ifndef KIRAN_CPANEL_NETWORK_MANAGER_H
#define KIRAN_CPANEL_NETWORK_MANAGER_H
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <QTimer>
#include <QWidget>
enum EditPages
{
    PAGE_SHOW,
    PAGE_SETTING
};

class Manager : public QWidget
{
    Q_OBJECT
public:
    explicit Manager(QWidget *parent = nullptr);
    ~Manager();

public slots:
    virtual void refreshConnectionLists();
    virtual void handleNotifierConnectionAdded(const QString &path) {};
    virtual void handleNotifierConnectionRemoved(const QString &path) {};
    virtual void handleActiveConnectionAdded(const QString &activepath) {};
    virtual void handleActiveConnectionRemoved(const QString &activepath) {};

    virtual void handleStateActivated(const QString &activatedPath) {};
    virtual void handleStateActivating(const QString &activatedPath) {};
    virtual void handleStateDeactivated(const QString &deactivatedPath) {};
    virtual void handleActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state);

    virtual void handleDeviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);

protected:
    QMap<QString, QString> m_deviceMap;
    QList<NetworkManager::Device::Ptr> m_deviceList;
    QString m_devicePath;
    NetworkManager::Device::Ptr m_devicePtr;

private:
    QTimer m_connectionTimer;
    QString m_connectionRemovePath;
};

#endif  // KIRAN_CPANEL_NETWORK_MANAGER_H
