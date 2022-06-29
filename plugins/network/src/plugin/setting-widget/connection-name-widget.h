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
#include <QWidget>
#include <QComboBox>
QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionNameWidget;
}
QT_END_NAMESPACE
using namespace NetworkManager;

enum VpnType
{
    VPN_TYPE_L2TP,
    VPN_TYPE_PPTP
};

class KiranSwitchButton;
class ConnectionNameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionNameWidget(QWidget *parent = nullptr);
    ~ConnectionNameWidget() override;
    void initUI();
    void setConnectionSettings(const ConnectionSettings::Ptr &connectionSettings);
    void setNameLabel(const QString &name);
    int connectionSuffixNum(QString &connName);
public slots:
    void saveSettings();
    void showSettings(ConnectionSettings::ConnectionType connectionType);
    void showVpnSettings(VpnType vpnType);
    void clearPtr();
    bool isInputValid();
private:
    Ui::ConnectionNameWidget *ui;
    ConnectionSettings::Ptr m_connectionSettings;
    ConnectionSettings::ConnectionType m_connectionType;
    KiranSwitchButton *m_autoConnection;
};
#endif  //KIRAN_CPANEL_NETWORK_CONNECTION_NAME_WIDGET_H
