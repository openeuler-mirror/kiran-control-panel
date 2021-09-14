/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "main-window.h"
#include <QListWidgetItem>
#include "ui_main-window.h"

MainWindow::MainWindow(QWidget* parent) : KiranTitlebarWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(getWindowContentWidget());
    addSidebarItem(tr("General Option"), ":/images/kck-page-generation.svg");
    addSidebarItem(tr("Keyboard Layout"), ":/images/kck-page-layout.png");

    connect(ui->listWidget, &KiranSidebarWidget::currentRowChanged,
            [this](int currentRow) {
                if (currentRow == KCP_LIST_ITEM_GENERAL)
                    ui->stackedWidget->setCurrentWidget(ui->page_general);
                else
                    ui->stackedWidget->setCurrentWidget(ui->page_layout);
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addSidebarItem(QString text, QString icon)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setIcon(QIcon(icon));
    item->setText(text);
    ui->listWidget->addItem(item);
}
