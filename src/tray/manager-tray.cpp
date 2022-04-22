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

#include "manager-tray.h"
#include "ui_manager-tray.h"

ManagerTray::ManagerTray(QWidget *parent) : QWidget(parent), ui(new Ui::ManagerTray)
{
    ui->setupUi(this);
    init();
}

ManagerTray::~ManagerTray()
{
    delete ui;
}

void ManagerTray::init()
{
    m_systemTray = new QSystemTrayIcon();
}

void ManagerTray::initConnect()
{

}
