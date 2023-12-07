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

#ifndef KIRAN_CPANEL_NETWORK_WIRED_SETTING_PAGE_H
#define KIRAN_CPANEL_NETWORK_WIRED_SETTING_PAGE_H

#include <QWidget>
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

    void initConnecton();
    void initSettingPage() override;
    void initSpecificSettings() override;
    void clearPtr() override;

    void createSettingPage(const QString &devicePath);

public slots:
    void saveSettingPage() override;
    bool isInputValid() override;
    void showSettingPage(QString activeConnectionPath = "");

private:
    Ui::WiredSettingPage *ui;
    KiranSwitchButton *m_security;
};

#endif  // KIRAN_CPANEL_NETWORK_WIRED_SETTING_PAGE_H
