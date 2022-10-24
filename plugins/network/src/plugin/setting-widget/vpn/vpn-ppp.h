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

#ifndef KIRAN_CPANEL_NETWORK_VPN_PPP_H
#define KIRAN_CPANEL_NETWORK_VPN_PPP_H

#include <NetworkManagerQt/VpnSetting>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnPpp;
}
QT_END_NAMESPACE

class KiranSwitchButton;
class VpnPpp : public QWidget
{
    Q_OBJECT

public:
    explicit VpnPpp(QWidget *parent = nullptr);
    ~VpnPpp() override;
    void initUI();
    void initConnection();
    void initOptionsButton(const QStringList &supportOptions);
    void initOptionsStrMap();
    void setVpnSetting(const NetworkManager::VpnSetting::Ptr &vpnSetting);

public slots:
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();

    void handleMPPEChanged(bool checked);

private:
    Ui::VpnPpp *ui;
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    QMap<QString,QString> m_optionsStrMap;
    QStringList m_supportOptions;
    QMap<QString,KiranSwitchButton*> m_optionsButtonMap;

    KiranSwitchButton *m_useMPPE;
    KiranSwitchButton *m_statefulMPPE;
};

#endif  //KIRAN_CPANEL_NETWORK_VPN_PPP_H
