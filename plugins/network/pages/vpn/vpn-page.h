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
namespace Ui
{
class VpnPage;
}

namespace Kiran
{
namespace Network
{
class Manager;
class VpnSettingItem;
class VpnPage : public Page
{
    Q_OBJECT
public:
    explicit VpnPage(QWidget *parent = 0);
    ~VpnPage();

    void reset() override;
private:
    void initUI();
    void init();
    VpnSettingItem* findConnectionItemByPath(const QString& connectionPath);
    VpnSettingItem* findConnectionItemByUUID(const QString& uuid);
    VpnSettingItem* findConnectionItem(std::function<bool(VpnSettingItem*,QString)> func,const QString& userData);

private slots:
    void appendVPNConnection(const QString& connectionPath);
    void createConnection();
    void connectionRemoved(const QString& connectionPath);

private:
    Ui::VpnPage *ui;
};

}  // namespace Network
}  // namespace Kiran
