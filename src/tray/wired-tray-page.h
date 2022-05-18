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

#ifndef KIRAN_CPANEL_NETWORK_WIRED_TRAY_H
#define KIRAN_CPANEL_NETWORK_WIRED_TRAY_H

#include <NetworkManagerQt/WiredDevice>
#include <QWidget>
#include "tray-page.h"

using namespace NetworkManager;
class ConnectionLists;
class WiredTrayPage : public TrayPage
{
    Q_OBJECT

public:
    explicit WiredTrayPage(QWidget *parent = nullptr);
    ~WiredTrayPage() override;

    void init();
    void initConnection();
    void initUI();

    void showWiredConnectionLists();
    void handleRequestActivateConnection(const QString &connectionPath, const QString &connectionParameter);

    void handleNotifierConnectionAdded(const QString &path);
    void handleNotifierConnectionRemoved(const QString &path);

    void handleStateDeactivated(const QString &deactivatedPath);
    void handleStateActivated(const QString &activatedPath);

    void handleActiveConnectionAdded(const QString &path);
    void handleActiveConnectionRemoved(const QString &path);


private:
    WiredDevice::Ptr m_wiredDevice;

    QPointer<ConnectionLists> m_connectionLists;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRED_TRAY_H
