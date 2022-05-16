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

#include "wired-tray-page.h"
#include "connection-lists.h"
WiredTrayPage::WiredTrayPage(QWidget *parent) : TrayPage(parent)
{
    init();
}

WiredTrayPage::~WiredTrayPage()
{
}
void WiredTrayPage::init()
{
    initUI();
}
void WiredTrayPage::initConnection()
{
}

void WiredTrayPage::initUI()
{
    setDeviceLabel(tr("Select wired network card"));
    showWiredConnectionLists();
}

void WiredTrayPage::showWiredConnectionLists()
{
    m_connectionLists->showConnectionLists(ConnectionSettings::Wired, ITEM_WIDGET_TYPE_TRAY);
    m_connectionLists->showWiredStatusIcon();
}
