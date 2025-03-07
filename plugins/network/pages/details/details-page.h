/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once
#include "page-manager.h"
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/ConnectionSettings>

namespace Ui
{
class DetailsPage;
}

namespace Kiran
{
namespace Network
{
typedef NetworkManager::ConnectionSettings::ConnectionType ConnectionType;
class DetailsPage : public Page
{
    Q_OBJECT
public:
    explicit DetailsPage(QWidget *parent = 0);
    ~DetailsPage();
    void reset() override;

private:
    void initUI();
    void init();
    void resetUI();

private slots:
    void processCurrentActiveConnChanged();
    void loadActiveConnectionInfo();
    void updateConnectionDeviceInfo();
    void loadWirelessInfo();
    void updateConnectionSecurityInfo();
    void updateConnectionIpDetails();
    
    void processActiveConnectionAdded(ConnectionType type, const QString& path, const QString& uuid);
    void processActiveConnectionRemoved(ConnectionType type, const QString& path, const QString& uuid);

private:
    Ui::DetailsPage *ui;
    NetworkManager::ActiveConnection::Ptr m_activeConnection;
};
}  // namespace Network
}  // namespace Kiran
