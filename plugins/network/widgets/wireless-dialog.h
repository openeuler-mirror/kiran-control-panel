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
#include "input-dialog/input-dialog.h"

namespace Kiran
{
namespace Network
{

class WirelessDialog : public InputDialog
{
    Q_OBJECT
    enum DialogType
    {
        WIRELESS_DIALOG_TYPE_PASSWD,
        WIRELESS_DIALOG_TYPE_HIDDEN_SSID,
    };
public:
    ~WirelessDialog();

    static bool getNetworkPasswd(QWidget* w,const QString& ssid, QString& passwd);
    static bool getHiddenNetworkSsid(QWidget* w,QString& ssid);

private:
    WirelessDialog(DialogType type, QWidget* parent = nullptr);
    void init();
    bool checkValid(const QString& text) override;

private:
    DialogType m_type;
};
}  // namespace Network
}  // namespace Kiran