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

#ifndef KIRAN_CPANEL_NETWORK_VPN_IPSEC_H
#define KIRAN_CPANEL_NETWORK_VPN_IPSEC_H

#include <QWidget>
#include <NetworkManagerQt/VpnSetting>
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnIPsec;
}
QT_END_NAMESPACE


class KiranSwitchButton;
class VpnIPsec : public QWidget
{
    Q_OBJECT

public:
    explicit VpnIPsec(QWidget *parent = nullptr);
    ~VpnIPsec() override;

    void initUI();
    void initConnection();
    void setVpnSetting(const NetworkManager::VpnSetting::Ptr &vpnSetting);

public slots:
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();

private:
    Ui::VpnIPsec *ui;
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    KiranSwitchButton *m_enableIPsec;
};

#endif  //KIRAN_CPANEL_NETWORK_VPN_IPSEC_H
