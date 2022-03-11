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
#ifndef KIRAN_CPANEL_NETWORK_COMM_SETTING_WIDGET_H
#define KIRAN_CPANEL_NETWORK_COMM_SETTING_WIDGET_H

#include <QWidget>
#include <QObject>
#include <NetworkManagerQt/Ipv4Setting>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CommSettingWidget;
}
QT_END_NAMESPACE

struct CommConfigInfo
{
    NetworkManager::Ipv4Setting::ConfigMethod ipv4Method = NetworkManager::Ipv4Setting::ConfigMethod::Automatic;
    QString ipv4Address = "";
    QString ipv4Netmask = "";
    QString ipv4Gateway = "";
};

Q_DECLARE_METATYPE(NetworkManager::Ipv4Setting::ConfigMethod)
enum SettingConnectionStatus
{
    SETTING_CONNECTION_STATUS_NEW,
    SETTING_CONNECTION_STATUS_DEACTIVATED,
    SETTING_CONNECTION_STATUS_ACTIVATED
};

class CommSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommSettingWidget(QWidget *parent = nullptr);
    ~CommSettingWidget() override;

    void initUI();
    void initConnection();

public slots:
    void handleIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method);
    CommConfigInfo getCommConfig();
    void initCommConfig(CommConfigInfo configInfo);
    void initDisconnectAndDeleteButton(SettingConnectionStatus connectionStatus);


private:
    Ui::CommSettingWidget *ui;
    CommConfigInfo m_configInfo;
};

#endif  //KIRAN_CPANEL_NETWORK_COMM_SETTING_WIDGET_H
