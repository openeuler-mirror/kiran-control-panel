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

#ifndef KIRAN_CPANEL_NETWORK_WIRED_MANAGER_H
#define KIRAN_CPANEL_NETWORK_WIRED_MANAGER_H

#include <NetworkManagerQt/WiredDevice>
#include <QWidget>
#include "manager.h"


QT_BEGIN_NAMESPACE
namespace Ui
{
class WiredManager;
}
QT_END_NAMESPACE

class WiredManager : public Manager
{
    Q_OBJECT

public:
    explicit WiredManager(const QString &devicePath, QWidget *parent = nullptr);
    ~WiredManager() override;

    void initUI();
    void initConnection();

public slots:
    void handleRequestActivateConnection(const QString &connectionPath, const QString &connectionParameter);

    void handleStateDeactivated(const QString &deactivatedPath) override;
    void handleStateActivated(const QString &activatedPath) override;
    void handleStateActivating(const QString &activatedPath) override;
    void handleReturnPreviousPage();

    void handleNotifierConnectionAdded(const QString &path) override;
    void handleNotifierConnectionRemoved(const QString &path) override;

    void handleActiveConnectionAdded(const QString &path) override;
    void handleActiveConnectionRemoved(const QString &path) override;

    void handleSaveButtonClicked();
    void handleConnectionUpdated(const QString &path);

private:
    Ui::WiredManager *ui;
    NetworkManager::WiredDevice::Ptr m_wiredDevice;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRED_MANAGER_H
