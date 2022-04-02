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

#include "cpanel-network-widget.h"
#include "ui_cpanel-network-widget.h"

enum NetworkSettingsPages
{
    PAGE_WIRED,
    PAGE_DSL,
    PAGE_VPN
};

CPanelNetworkWidget::CPanelNetworkWidget(QWidget* parent) : QWidget(parent), ui(new Ui::CPanelNetworkWidget)
{
    ui->setupUi(this);
    init();

    ui->wiredPage;
}

CPanelNetworkWidget::~CPanelNetworkWidget()
{
    delete ui;
}

void CPanelNetworkWidget::init()
{
    ui->sidebar->insertItem(PAGE_WIRED, tr("Wired Network"));
    ui->sidebar->insertItem(PAGE_DSL, tr("DSL"));
    ui->sidebar->insertItem(PAGE_VPN, tr("VPN"));


    ui->sidebar->item(PAGE_WIRED)->setData(Qt::UserRole, PAGE_WIRED);
    ui->sidebar->item(PAGE_DSL)->setData(Qt::UserRole, PAGE_DSL);
    ui->sidebar->item(PAGE_VPN)->setData(Qt::UserRole, PAGE_VPN);

    connect(ui->sidebar, &QListWidget::currentItemChanged, [this](QListWidgetItem* current, QListWidgetItem* previous) {
        ui->stackedWidget->setCurrentIndex(current->data(Qt::UserRole).toInt());
    });
}
