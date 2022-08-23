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

#ifndef KIRAN_CPANEL_NETWORK_VPN_L2TP_SETTING_H
#define KIRAN_CPANEL_NETWORK_VPN_L2TP_SETTING_H

#include <NetworkManagerQt/VpnSetting>
#include <QWidget>
#include "setting-page.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnL2tpSetting;
}
QT_END_NAMESPACE

class VpnL2tpSetting : public SettingPage
{
    Q_OBJECT

public:
    explicit VpnL2tpSetting(QWidget *parent = nullptr);
    ~VpnL2tpSetting() override;

    void initConnection();
    void initSettingPage() override;
    void initSpecificSettings() override;
    void clearPtr() override;

public slots:
    void saveSettingPage() override;
    bool isInputValid() override;
    void showSettingPage(QString activeConnectionPath = "");

private:
    Ui::VpnL2tpSetting *ui;
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
};

#endif  // KIRAN_CPANEL_NETWORK_VPN_L2TP_SETTING_H
