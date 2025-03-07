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
#include <QWidget>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Connection>

namespace Ui
{
class ConnectionSettingsPage;
}

class KiranTips;
namespace Kiran
{
namespace Network
{
class ConnectionSettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionSettingsPage(QWidget *parent = 0);
    ~ConnectionSettingsPage();

    void reset();
    void editConnectionSettings(const QString& devicePath,const QString& connectionPath);

private:
    void init();
    QString newConnectionName();
    void loadSettingsInfo();
    void loadDeviceComboBox();
    void setCurrentBoundDevice(const QString& boundInterface,const QString& boundAddress);
    bool macValid(const QString& mac);

private slots:
    void save();

private:
    Ui::ConnectionSettingsPage *ui;
    // 已有配置会有Connection
    NetworkManager::Connection::Ptr m_existingConnection;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;
    KiranTips* m_tips;
};

}  // namespace Network
}  // namespace Kiran