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
#ifndef KIRAN_CPANEL_NETWORK_VPN_WIDGET_H
#define KIRAN_CPANEL_NETWORK_VPN_WIDGET_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/VpnSetting>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnWidget;
}

QT_END_NAMESPACE
Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType);

class KiranTips;
class VpnWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VpnWidget(QWidget *parent = nullptr);
    ~VpnWidget() override;
    void initUI();
    void initConnection();
    void setVpnSetting(const NetworkManager::VpnSetting::Ptr &vpnSetting);
    void setConnectionPtr(const NetworkManager::Connection::Ptr &connection);
    void setErrorTips(KiranTips *errorTips);
    bool isIpv4AddressValid(const QString &address);

public slots:
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();

    bool isInputValid();
    void handlePasswordOptionsChanged(NetworkManager::Setting::SecretFlagType secretFlagType);
    void enablePasswordVisual();

private:
    Ui::VpnWidget *ui;
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NetworkManager::Connection::Ptr m_connection;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;
    KiranTips *m_errorTip = nullptr;
};

#endif  // KIRAN_CPANEL_NETWORK_VPN_WIDGET_H
