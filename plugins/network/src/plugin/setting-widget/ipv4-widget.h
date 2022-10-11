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
#ifndef KIRAN_CPANEL_NETWORK_IPV4_WIDGET_H
#define KIRAN_CPANEL_NETWORK_IPV4_WIDGET_H

#include <NetworkManagerQt/Ipv4Setting>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Ipv4Widget;
}
QT_END_NAMESPACE

Q_DECLARE_METATYPE(NetworkManager::Ipv4Setting::ConfigMethod)

class KiranTips;
class Ipv4Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Ipv4Widget(QWidget *parent = nullptr);
    ~Ipv4Widget() override;
    void initUI();
    void initConnection();
    void setIpv4Setting(const NetworkManager::Ipv4Setting::Ptr &ipv4Setting);
    void setErrorTips(KiranTips *errorTips);
    bool isIpv4AddressValid(const QString &address);
    bool isIpv4NetmaskValid(const QString &address);

public slots:
    void handleIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method);
    void resetSettings();
    void saveSettings();
    void showSettings();
    void clearPtr();
    bool isInputValid();

private:
    Ui::Ipv4Widget *ui;
    NetworkManager::Ipv4Setting::Ptr m_ipv4Setting;
    KiranTips *m_errorTip = nullptr;
};

#endif  // KIRAN_CPANEL_NETWORK_IPV4_WIDGET_H
