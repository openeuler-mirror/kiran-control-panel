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

#ifndef KIRAN_CPANEL_NETWORK_GENERAL_WIDGET_H
#define KIRAN_CPANEL_NETWORK_GENERAL_WIDGET_H

#include <NetworkManagerQt/ConnectionSettings>
#include <QWidget>
#include <QComboBox>
QT_BEGIN_NAMESPACE
namespace Ui
{
class GeneralWidget;
}
QT_END_NAMESPACE
using namespace NetworkManager;

enum VpnType
{
    VPN_TYPE_L2TP,
    VPN_TYPE_PPTP
};

class KiranSwitchButton;
class GeneralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralWidget(QWidget *parent = nullptr);
    ~GeneralWidget() override;
    void initUI();
    void setConnectionSettings(const ConnectionSettings::Ptr &connectionSettings);
    void setNameLabel(const QString &name);
    int connectionSuffixNum(QString &connName);
public slots:
    void saveSettings();
    void showSettings(ConnectionSettings::ConnectionType connectionType);
    void showVpnSettings(VpnType vpnType);
    void clearPtr();
private:
    Ui::GeneralWidget *ui;
    ConnectionSettings::Ptr m_connectionSettings;
    ConnectionSettings::ConnectionType m_connectionType;
    KiranSwitchButton *m_autoConnection;
};

class GeneralComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit GeneralComboBox(QWidget *parent = nullptr);

    void wheelEvent(QWheelEvent *e) override;
};

#endif  //KIRAN_CPANEL_NETWORK_GENERAL_WIDGET_H
