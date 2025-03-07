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
#include "wireless-dialog.h"

namespace Kiran
{
namespace Network
{
WirelessDialog::WirelessDialog(DialogType type, QWidget* parent)
    : InputDialog(parent),
      m_type(type)
{
    init();
}

WirelessDialog::~WirelessDialog()
{
}

bool WirelessDialog::getNetworkPasswd(QWidget* w, const QString& ssid, QString& passwd)
{
    WirelessDialog dialog(WirelessDialog::WIRELESS_DIALOG_TYPE_PASSWD, w);
    dialog.setTitle(tr("WIFI password"));
    QString desc = QString(tr("Password required to connect to %1.")).arg(ssid);
    dialog.setDesc(desc);
    dialog.setInputMode(QLineEdit::Password, 64);

    if (dialog.exec())
    {
        passwd = dialog.getText();
        return true;
    }
    return false;
}

bool WirelessDialog::getHiddenNetworkSsid(QWidget* w, QString& ssid)
{
    WirelessDialog dialog(WirelessDialog::WIRELESS_DIALOG_TYPE_HIDDEN_SSID, w);
    dialog.setTitle(tr("Hidden wireless network ssid"));
    QString desc = tr("Please enter the hidden wireless network ssid");
    dialog.setDesc(desc);
    dialog.setInputMode(QLineEdit::Normal, 32);

    if (dialog.exec())
    {
        ssid = dialog.getText();
        return true;
    }
    return false;
}

bool WirelessDialog::checkValid(const QString& text)
{
    switch (m_type)
    {
    case WIRELESS_DIALOG_TYPE_PASSWD:
    {
        if (text.size() < 8)
            return false;
        break;
    }
    case WIRELESS_DIALOG_TYPE_HIDDEN_SSID:
    {
        if( text.size() < 1 )
            return false;
        break;
    }
    default:
        break;
    }
    return true;
}

void WirelessDialog::init()
{
}

}  // namespace Network
}  // namespace Kiran
