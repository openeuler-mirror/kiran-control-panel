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

#ifndef KIRAN_CPANEL_NETWORK_VPN_MANAGER_H
#define KIRAN_CPANEL_NETWORK_VPN_MANAGER_H

#include <NetworkManagerQt/VpnConnection>
#include <QWidget>
#include "manager.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnManager;
}
QT_END_NAMESPACE

class VpnManager : public Manager
{
    Q_OBJECT

public:
    explicit VpnManager(QWidget *parent = nullptr);
    ~VpnManager() override;

    void initUI();
    void initConnection();

public slots:
    void clearVpnSetting();
    void handleCreatConnection();
    void handleEditConnection(const QString &uuid, QString activeConnectionPath);
    void handleActivateSelectedConnection(const QString &connectionPath, const QString &connectionParameter);

    void handleNotifierConnectionAdded(const QString &path) override;
    void handleNotifierConnectionRemoved(const QString &path) override;
    void activateVPNConnection(const QString &connectionPath, const QString &connectionParameter);

    void handleActiveConnectionAdded(const QString &activePath) override;
    void handleActiveConnectionRemoved(const QString &activePath) override;

    void handleVpnConnectionStateChanged(NetworkManager::VpnConnection::State state, NetworkManager::VpnConnection::StateChangeReason reason);
    void handleStateActivated(const QString &activePath);
    void handleStateActivating(const QString &activePath) override;

    void handleVpnStateDisconnected(const QString &activePath);
    void handleVpnStateFailed(const QString &activePath);

    void handleReturnPreviousPage();

    void handleConnectionUpdated(const QString &path);
    void handleSaveButtonClicked();

private:
    Ui::VpnManager *ui;
    QString m_devicePath;
};

#endif  // KIRAN_CPANEL_NETWORK_VPN_MANAGER_H
