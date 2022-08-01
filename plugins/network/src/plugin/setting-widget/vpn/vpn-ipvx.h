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

#ifndef KIRAN_CPANEL_NETWORK_VPN_IPVX_H
#define KIRAN_CPANEL_NETWORK_VPN_IPVX_H

#include <QWidget>
#include <NetworkManagerQt/Ipv4Setting>
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnIpvx;
}
QT_END_NAMESPACE

class KiranSwitchButton;

class VpnIpvx : public QWidget
{
    Q_OBJECT
public:
    explicit VpnIpvx(QWidget *parent = nullptr);
    ~VpnIpvx() override;

    void initUI();
    void initConnection();
    void setIpv4Setting(const NetworkManager::Ipv4Setting::Ptr &ipv4Setting);

public slots:
    void saveSettings();
    void showSeittngs();
    void resetSettings();

    void clearPtr();

private:
    Ui::VpnIpvx *ui;
    KiranSwitchButton *m_neverDefault;
    NetworkManager::Ipv4Setting::Ptr m_ipv4Setting;
};



#endif  //KIRAN_CPANEL_NETWORK_VPN_IPVX_H
