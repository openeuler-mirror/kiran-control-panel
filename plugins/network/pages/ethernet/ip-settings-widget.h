/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>
#include <QWidget>

namespace Ui
{
class IpSettingsWidget;
}

class KiranTips;
namespace Kiran
{
namespace Network
{
// NetworkManager::Ipv4Setting NetworkManager::Ipv6Setting
// 配置界面
class IpSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    enum Type
    {
        TYPE_IPV4,
        TYPE_IPV6,
        TYPE_LAST
    };
    explicit IpSettingsWidget(QWidget* parent = 0);
    ~IpSettingsWidget();
    
    void setIpSettingsType(Type type);

    // 设置父控件的Tips控件，用于CheckValid时的界面报错提示
    void setTipsWidget(KiranTips* tips);
    void reset();

    // 加载配置
    void loadSettings(NetworkManager::Ipv4Setting::Ptr setting);
    void loadSettings(NetworkManager::Ipv6Setting::Ptr setting);

    // 检查当前UI配置地址是否有效
    bool checkValid();
    // 保存IP配置，调用前需checkValid
    void save();

private:
    void init();

    // 加载默认以及配置中ConfigMethod初始化下拉框
    void loadConfigMethodComboBox();

    // 检查配置界面元素是否应在当前ConfigMethod下显示
    bool isStaticIPConfigUIVisible(NetworkManager::Ipv4Setting::ConfigMethod method);
    bool isStaticIPConfigUIVisible(NetworkManager::Ipv6Setting::ConfigMethod method);
    bool isDNSConfigUIVisible(NetworkManager::Ipv4Setting::ConfigMethod method);
    bool isDNSConfigUIVisible(NetworkManager::Ipv6Setting::ConfigMethod method);
    
    // 检查界面IPv4配置是否有效
    bool checkIpv4Valid();
    bool ipv4AddressValid(const QString& address);
    bool ipv4MaskValid(const QString& mask);

    // 检查界面IPv6配置是否有效
    bool checkIpv6Valid();
    bool ipv6AddressValid(const QString& address);
    bool ipv6PrefixValid(const QString& prefix);

    // 保存相应配置项
    void saveIpv4Settings();
    void saveIpv6Settings();

private slots:
    void processConfigMethodChanged();

private:
    ::Ui::IpSettingsWidget* ui;
    KiranTips* m_tips = nullptr;
    Type m_type = TYPE_LAST;
    NetworkManager::Ipv4Setting::Ptr m_ipv4Setting;
    NetworkManager::Ipv6Setting::Ptr m_ipv6Setting;
};

}  // namespace Network
}  // namespace Kiran