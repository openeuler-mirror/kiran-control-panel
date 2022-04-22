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

#ifndef KIRAN_CPANEL_NETWORK_WIRELESS_SETTING_PAGE_H
#define KIRAN_CPANEL_NETWORK_WIRELESS_SETTING_PAGE_H

#include <QWidget>
#include "setting-page.h"
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
QT_BEGIN_NAMESPACE
namespace Ui
{
class WirelessSettingPage;
}
QT_END_NAMESPACE
using namespace NetworkManager;
class WirelessSettingPage : public SettingPage
{
    Q_OBJECT

public:
    explicit WirelessSettingPage(QWidget *parent = nullptr);
    ~WirelessSettingPage() override;
    void initConnection();
    void initSettingPage() override;
    void initSpecificSettings() override;
    void initWidgets() override;
    void clearPtr() override;

public slots:
    void saveSettingPage() override;
    void showSettingPage(QString activeConnectionPath = "");
private:
    Ui::WirelessSettingPage *ui;
    WirelessSetting::Ptr m_wirelessSetting;
    WirelessSecuritySetting::Ptr m_wirelessSecuritySetting;
};

#endif  //KIRAN_CPANEL_NETWORK_WIRELESS_SETTING_PAGE_H
