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

#ifndef KIRAN_CPANEL_NETWORK_IPV6_WIDGET_H
#define KIRAN_CPANEL_NETWORK_IPV6_WIDGET_H

#include <NetworkManagerQt/Ipv6Setting>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class Ipv6Widget;
}
QT_END_NAMESPACE

Q_DECLARE_METATYPE(NetworkManager::Ipv6Setting::ConfigMethod)

class Ipv6Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Ipv6Widget(QWidget *parent = nullptr);
    ~Ipv6Widget() override;
    void initUI();
    void initConnection();
    void setIpv6Setting(const NetworkManager::Ipv6Setting::Ptr &ipv6Setting);

    bool isIpv6AddressValid(const QString &address);

public slots:
    void handleIpv6MethodChanged(NetworkManager::Ipv6Setting::ConfigMethod method);
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();
    bool isInputValid();

private:
    Ui::Ipv6Widget *ui;
    NetworkManager::Ipv6Setting::Ptr m_ipv6Setting;
};

#endif  // KIRAN_CPANEL_NETWORK_IPV6_WIDGET_H
