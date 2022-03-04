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
#include "wired-page.h"
#include "ui_wired-page.h"

#include <NetworkManagerQt/Connection>

enum WiredEditPages
{
    PAGE_WIRED,
    PAGE_WIRED_SETTING
};

WiredPage::WiredPage(QWidget *parent) : QWidget(parent), ui(new Ui::WiredPage)
{
    ui->setupUi(this);
    init();
    initConnecton();
}

WiredPage::~WiredPage()
{
    delete ui;
}

void WiredPage::init()
{
    ui->connectionEditPage->setTitle(tr("Wired Network Adapter"));
    ui->connectionEditPage->setSwitchButtonVisible(true);
}

void WiredPage::initConnecton()
{
    connect(ui->connectionEditPage,&ConnectionEditPage::requestCreatConnection,[=](){
      ui->stackedWidget->setCurrentIndex(PAGE_WIRED_SETTING);
    });

    connect(ui->wiredSettingPage,&WiredSettingPage::returnWiredEditPage,[=](){
       ui->stackedWidget->setCurrentIndex(PAGE_WIRED);
    });

}
