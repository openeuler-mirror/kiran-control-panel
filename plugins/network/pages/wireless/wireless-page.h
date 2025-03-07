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
#include "page-manager.h"
class KiranPushButton;

namespace Ui
{
class WirelessPage;
}

namespace Kiran
{
namespace Network
{
class WirelessPage : public Page
{
    Q_OBJECT
    
public:
    explicit WirelessPage(QWidget *parent = 0);
    ~WirelessPage();

    void reset() override;
private:
    void init();
    QWidget* findDeviceWidget(const QString& devicePath);

private slots:
    void processManagedDeviceAdded(const QString& devicePath);
    void processManagedDeviceRemoved(const QString& devicePath);
    void jumpToManageWirelessNetworkSettings();

private:
    Ui::WirelessPage *ui;
    KiranPushButton* m_knownNetworksBtn;
};
}  // namespace Network
}  // namespace Kiran