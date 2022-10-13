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

#ifndef KIRAN_CPANEL_NETWORK_WIRELESS_SECURITY_WIDGET_H
#define KIRAN_CPANEL_NETWORK_WIRELESS_SECURITY_WIDGET_H

#include <NetworkManagerQt/Security8021xSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class WirelessSecurityWidget;
}
QT_END_NAMESPACE

Q_DECLARE_METATYPE(NetworkManager::WirelessSecuritySetting::KeyMgmt)
class WirelessSecurityWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WirelessSecurityWidget(QWidget *parent = nullptr);
    ~WirelessSecurityWidget() override;

    void initUI();
    void initConnection();
    void setWirelessSecuritySetting(const NetworkManager::WirelessSecuritySetting::Ptr &wirelessSecuritySetting);

public slots:
    void handleSecurityOptionChanged(NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt);
    void handlePasswordOptionsChanged(NetworkManager::Setting::SecretFlagType secretFlagType);
    void enablePasswordVisual();
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();

private:
    Ui::WirelessSecurityWidget *ui;
    NetworkManager::WirelessSecuritySetting::Ptr m_wirelessSecuritySetting;
    NetworkManager::Security8021xSetting::Ptr m_security8021xSetting;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRELESS_SECURITY_WIDGET_H
