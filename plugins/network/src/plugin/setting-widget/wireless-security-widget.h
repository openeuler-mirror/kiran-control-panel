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

#ifndef KIRAN_CPANEL_NETWORK_WIRELESS_SECURITY_WIDGET_H
#define KIRAN_CPANEL_NETWORK_WIRELESS_SECURITY_WIDGET_H

#include <QWidget>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/Security8021xSetting>
QT_BEGIN_NAMESPACE
namespace Ui
{
class WirelessSecurityWidget;
}
QT_END_NAMESPACE
using namespace NetworkManager;
Q_DECLARE_METATYPE(NetworkManager::WirelessSecuritySetting::KeyMgmt)
class WirelessSecurityWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WirelessSecurityWidget(QWidget *parent = nullptr);
    ~WirelessSecurityWidget() override;

    void initUI();
    void initConnection();
    void setWirelessSecuritySetting(const WirelessSecuritySetting::Ptr &wirelessSecuritySetting);

public slots:
    void handleSecurityOptionChanged(WirelessSecuritySetting::KeyMgmt keyMgmt);
    void handlePasswordOptionsChanged(Setting::SecretFlagType secretFlagType);
    void enablePasswordVisual();
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();
private:
    Ui::WirelessSecurityWidget *ui;
    WirelessSecuritySetting::Ptr m_wirelessSecuritySetting;
    Security8021xSetting::Ptr m_security8021xSetting;
};

#endif  //KIRAN_CPANEL_NETWORK_WIRELESS_SECURITY_WIDGET_H
