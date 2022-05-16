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

#ifndef KIRAN_CPANEL_NETWORK_MANAGER_H
#define KIRAN_CPANEL_NETWORK_MANAGER_H
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <QWidget>
#include <QTimer>
enum EditPages
{
    PAGE_SHOW,
    PAGE_SETTING
};
using namespace NetworkManager;

class Manager : public QWidget
{
    Q_OBJECT
public:
    explicit Manager(QWidget *parent = nullptr);
    ~Manager();

    void initNotifierConnection();

public slots:
    virtual void refreshConnectionLists();
    virtual void handleNotifierConnectionAdded(const QString &path);
    virtual void handleNotifierConnectionRemoved(const QString &path);
    virtual void handleActiveConnectionAdded(const QString &activepath);
    virtual void handleActiveConnectionRemoved(const QString &activepath);

    virtual void handleActiveConnectionStateChanged(ActiveConnection::State state, const QString &path);
    virtual void handleStateActivated(const QString &activatedPath);
    virtual void handleStateDeactivated(const QString &deactivatedPath);
    void getDeviceList(Device::Type deviceType);


protected:
    QMap<QString, QString> m_deviceMap;
    QList<Device::Ptr> m_deviceList;

private:
    QTimer m_connectionTimer;
    QString m_connectionRemovePath;
};

#endif  //KIRAN_CPANEL_NETWORK_MANAGER_H
