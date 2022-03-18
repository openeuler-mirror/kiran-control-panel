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

#include "dsl-page.h"
#include <qt5-log-i.h>
#include "ui_dsl-page.h"

enum EditPages
{
    PAGE_SHOW,
    PAGE_SETTING
};

DslPage::DslPage(QWidget *parent) : QWidget(parent), ui(new Ui::DslPage)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

DslPage::~DslPage()
{
    delete ui;
}

void DslPage::initUI()
{
    ui->connectionShowPage->setTitle(tr("DSL"));
    ui->connectionShowPage->setSwitchButtonVisible(false);

    ui->dslSettingPage->initConnectionSettings(ConnectionSettings::ConnectionType::Pppoe);
}

void DslPage::initConnection()
{
    connect(ui->connectionShowPage, &ConnectionShowPage::requestCreatConnection, [=]() {
        ui->stackedWidget->setCurrentIndex(PAGE_SETTING);
    });

    connect(ui->dslSettingPage, &DslSettingPage::returnPreviousPage, [=]() {
        ui->stackedWidget->setCurrentIndex(PAGE_SHOW);
    });
}
