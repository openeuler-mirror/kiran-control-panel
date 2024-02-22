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

#ifndef KIRAN_CPANEL_NETWORK_WIRELESS_MANAGER_H
#define KIRAN_CPANEL_NETWORK_WIRELESS_MANAGER_H

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>
#include <QWidget>
#include "connection-list.h"
#include "manager.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class WirelessManager;
}
QT_END_NAMESPACE

class WirelessManager : public Manager
{
    Q_OBJECT

public:
    explicit WirelessManager(const QString &devicePath, QWidget *parent = nullptr);
    ~WirelessManager() override;

public slots:
    void editConnection(const QString &uuid, QString activeConnectionPath);
    void returnPreviousPage();

private:
    void initUI();
    void initConnection();

private:
    Ui::WirelessManager *ui;
    NetworkManager::WirelessDevice::Ptr m_wirelessDevice;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRELESS_MANAGER_H
