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

#ifndef KIRAN_CPANEL_NETWORK_CONNECTION_NAME_WIDGET_H
#define KIRAN_CPANEL_NETWORK_CONNECTION_NAME_WIDGET_H

#include <NetworkManagerQt/ConnectionSettings>
#include <QComboBox>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionNameWidget;
}
QT_END_NAMESPACE

enum VpnType
{
    VPN_TYPE_L2TP,
    VPN_TYPE_PPTP
};

class KiranSwitchButton;
class KiranTips;
class ConnectionNameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionNameWidget(QWidget *parent = nullptr);
    ~ConnectionNameWidget() override;
    void initUI();
    void setConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void setErrorTips(KiranTips *errorTips);
    void setNameLabel(const QString &name);
    int connectionSuffixNum(QString &connName);
public slots:
    void saveSettings();
    void showSettings(NetworkManager::ConnectionSettings::ConnectionType connectionType);
    void showVpnSettings(VpnType vpnType);
    void clearPtr();
    bool isInputValid();

private:
    Ui::ConnectionNameWidget *ui;
    KiranSwitchButton *m_autoConnection;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;
    NetworkManager::ConnectionSettings::ConnectionType m_connectionType;
    KiranTips *m_errorTip = nullptr;
};
#endif  // KIRAN_CPANEL_NETWORK_CONNECTION_NAME_WIDGET_H
