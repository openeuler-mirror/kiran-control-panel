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

#include "dsl-manager.h"
#include <qt5-log-i.h>
#include "ui_dsl-manager.h"

DslManager::DslManager(QWidget *parent) : Manager(parent), ui(new Ui::DslManager)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

DslManager::~DslManager()
{
    delete ui;
}

void DslManager::initUI()
{
    ui->connectionShowPage->setTitle(tr("DSL"));
    ui->connectionShowPage->setSwitchButtonVisible(false);
}

void DslManager::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });
}
