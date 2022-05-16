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
#ifndef KIRAN_CPANEL_NETWORK_MANAGER_TRAY_H
#define KIRAN_CPANEL_NETWORK_MANAGER_TRAY_H

#include <QSystemTrayIcon>
#include <QWidget>
#include <QVBoxLayout>
class WiredTrayPage;
class WirelessTrayPage;
class StatusNotifierManagerInterface;
class ManagerTray : public QWidget
{
    Q_OBJECT

public:
    explicit ManagerTray(QWidget *parent = nullptr);
    ~ManagerTray() override;
    void init();

    void initConnect();
    void initUI();
    void initTrayIcon();
    void initMenu();
    void initWiredTrayPage();
    void initWirelessTrayPage();

    void getTrayGeometry();
    void setTrayPagePos();

public:
    void handleTrayClicked(QSystemTrayIcon::ActivationReason reason);
    void showTrayPage();
    void setTrayIcon();

private:
    QSystemTrayIcon *m_systemTray;
    WiredTrayPage *m_wiredTrayPage;
    WirelessTrayPage *m_wirelessTrayPage;
    StatusNotifierManagerInterface *m_statusNotifierManager;

    QVBoxLayout *m_verticalLayout;
    int m_xTray, m_yTray, m_heightTray, m_widthTray;
};

#endif  // KIRAN_CPANEL_NETWORK_MANAGER_TRAY_H
