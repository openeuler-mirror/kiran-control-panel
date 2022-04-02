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
#ifndef KIRAN_CPANEL_NETWORK_VPN_WIDGET_H
#define KIRAN_CPANEL_NETWORK_VPN_WIDGET_H

#include <NetworkManagerQt/VpnSetting>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnWidget;
}
using namespace NetworkManager;
QT_END_NAMESPACE
Q_DECLARE_METATYPE(Setting::SecretFlagType);
class VpnWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VpnWidget(QWidget *parent = nullptr);
    ~VpnWidget() override;
    void initUI();
    void initConnection();
    void setVpnSetting(const VpnSetting::Ptr &vpnSetting);

public slots:
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();

    void handlePasswordOptionsChanged(Setting::SecretFlagType secretFlagType);
    void enablePasswordVisual();

private:
    Ui::VpnWidget *ui;
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;
};

#endif  //KIRAN_CPANEL_NETWORK_VPN_WIDGET_H
