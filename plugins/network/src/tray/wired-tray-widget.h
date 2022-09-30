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

#ifndef KIRAN_CPANEL_NETWORK_WIRED_TRAY_WIDGET_H
#define KIRAN_CPANEL_NETWORK_WIRED_TRAY_WIDGET_H

#include <NetworkManagerQt/WiredDevice>
#include <QWidget>
#include "tray-connection-list.h"
#include "tray-widget.h"

class TrayConnectionList;
class WiredTrayWidget : public TrayWidget
{
    Q_OBJECT

public:
    explicit WiredTrayWidget(const QString &devicePath, QWidget *parent = nullptr);
    ~WiredTrayWidget() override;

    void init();
    void initConnection();
    void initUI();

    void showWiredConnectionLists();
    void handleActivateSelectedConnection(const QString &connectionPath, const QString &connectionParameter);
    void handleDisconnect(const QString &activatedConnectionPath);
    void handleCancelConnection(const QString &activatedConnectionPath);

    void handleNotifierConnectionAdded(const QString &path) override;
    void handleNotifierConnectionRemoved(const QString &path) override;

    void handleStateDeactivated(const QString &activatedPath) override;
    void handleStateActivated(const QString &activatedPath) override;
    void handleStateActivating(const QString &activatedPath) override;

    void handleActiveConnectionAdded(const QString &path) override;
    void handleActiveConnectionRemoved(const QString &path) override;

    void handleConnectionUpdated(const QString &path);

    void initUnavailableWidget();

    int getHeight() override;

public slots:
    void handleCarrierChanged(bool plugged);
    void handleStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);

private:
    NetworkManager::WiredDevice::Ptr m_wiredDevice;
    QWidget *m_unavailableWidget;
    QString m_devicePath;
    QPointer<TrayConnectionList> m_connectionList;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRED_TRAY_WIDGET_H
