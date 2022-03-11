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

#ifndef KIRAN_CPANEL_NETWORK_WIRED_SETTING_PAGE_H
#define KIRAN_CPANEL_NETWORK_WIRED_SETTING_PAGE_H

#include <QWidget>

#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WiredSetting>
#include "comm-setting-widget.h"
#include "setting-page.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class WiredSettingPage;
}
QT_END_NAMESPACE
class KiranSwitchButton;

class WiredSettingPage : public SettingPage
{
    Q_OBJECT

public:
    explicit WiredSettingPage(QWidget *parent = nullptr);
    ~WiredSettingPage() override;

    void init();
    void initConnecton();
    void initUI();

    void configureConnection();

public slots:
    void handleSaveButtonClicked();
    void refreshSettingPage(QString activeConnectionPath="");

signals:

private:
    Ui::WiredSettingPage *ui;

    NetworkManager::WiredSetting::Ptr m_wiredSetting;
    NetworkManager::WiredDevice::Ptr m_wiredDevice;

    KiranSwitchButton *m_autoConnection;
    KiranSwitchButton *m_security;

    CommConfigInfo m_wiredCommConfig;
};

#endif  //KIRAN_CPANEL_NETWORK_WIRED_SETTING_PAGE_H
