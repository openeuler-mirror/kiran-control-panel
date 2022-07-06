/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-audio is licensed under Mulan PSL v2.
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

#include "cpanel-audio-widget.h"
#include "ui_cpanel-audio-widget.h"

#include <qt5-log-i.h>

enum AudioSettingsPages
{
    PAGE_OUTPUT,
    PAGE_INPUT,
    PAGE_SYSTEM_SOUND
};

CPanelAudioWidget::CPanelAudioWidget(QWidget* parent) : QWidget(parent), ui(new Ui::CPanelAudioWidget)
{
    ui->setupUi(this);
    init();
}

CPanelAudioWidget::~CPanelAudioWidget()
{
    delete ui;
}

void CPanelAudioWidget::init()
{
    ui->sidebar->insertItem(PAGE_OUTPUT, tr("Output"));
    ui->sidebar->insertItem(PAGE_INPUT, tr("Input"));

    ui->sidebar->item(PAGE_OUTPUT)->setData(Qt::UserRole, PAGE_OUTPUT);
    ui->sidebar->item(PAGE_INPUT)->setData(Qt::UserRole, PAGE_INPUT);

    connect(ui->sidebar, &QListWidget::currentItemChanged, [this](QListWidgetItem* current, QListWidgetItem* previous) {
        ui->stackedWidget->setCurrentIndex(current->data(Qt::UserRole).toInt());
    });
}
