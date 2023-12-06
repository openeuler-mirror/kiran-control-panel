/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */

#include "list-expansion-space.h"
#include <qt5-log-i.h>
#include "logging-category.h"
#include "ui_list-expansion-space.h"

ListExpansionSpace::ListExpansionSpace(QWidget *parent) : QWidget(parent),
                                                          ui(new Ui::ListExpansionSpace)
{
    ui->setupUi(this);
    init();
}

ListExpansionSpace::~ListExpansionSpace()
{
    delete ui;
}

void ListExpansionSpace::init()
{
    QPalette qPalette = this->palette();
    qPalette.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255, 0)));
    ui->listWidget->setPalette(qPalette);
    ui->listWidget->setFrameShape(QListWidget::NoFrame);
}

void ListExpansionSpace::addListExpansionSpaceItem(QWidget *widget)
{
    auto item = new QListWidgetItem();
    item->setSizeHint(widget->sizeHint());
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, widget);
    emit addedListWidgetItem();
}

void ListExpansionSpace::removeListExpansionSpaceCurrentItem()
{
    auto curItem = ui->listWidget->currentItem();
    ui->listWidget->removeItemWidget(curItem);
    delete curItem;

    emit removedListWidgetItem();
}

void ListExpansionSpace::removeListExpansionSpaceItem(int index)
{
    if (index < 0 || index >= ui->listWidget->count())
    {
        KLOG_WARNING(qLcCommonWidget) << "remove ListExpansionSpaceItem Failed, Invalid index value: " << index;
        return;
    }
    QListWidgetItem *pItem = ui->listWidget->item(index);
    ui->listWidget->removeItemWidget(pItem);
    delete pItem;
}
