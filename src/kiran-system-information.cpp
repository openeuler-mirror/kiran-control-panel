/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
 */

#include "kiran-system-information.h"
#include "ui_kiran-system-information.h"

#include <QPainter>
#include <QStackedWidget>
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QScroller>

kiranSystemInformation::kiranSystemInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::kiranSystemInformation)
{
    ui->setupUi(this);
    initUI();

    connect(ui->infoListWidget, SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(changeWidgetWhenItemClicked(QListWidgetItem*)));
}

kiranSystemInformation::~kiranSystemInformation()
{
    delete ui;
}

void kiranSystemInformation::initUI()
{
    setWindowFlags(Qt::FramelessWindowHint);
    /*创建左侧列表*/
    QString systemInfomationIcon = ":/images/system-information.svg";
    QString hardwareInformationIcon = ":/images/hardware-information.svg";
    systemInfomationItem = createInformationItem(QString(tr("System Information")) , systemInfomationIcon);
    hardwareInformationItem = createInformationItem(QString(tr("Hardware Information")) , hardwareInformationIcon);
    ui->infoListWidget->setCurrentRow(0);
}


/**
 * @brief  创建左侧自定义信息列表项
 * @param  text 列表项的名字
 * @return InfomationListItem 返回设计好的信息列表项
 */
InformationListItem *kiranSystemInformation::createInformationItem(const QString text , const QString iconPath)
{
    QListWidgetItem* newItem = nullptr;
    InformationListItem* customItem = nullptr;

    newItem = new QListWidgetItem(ui->infoListWidget);
    customItem = new InformationListItem(ui->infoListWidget);

    newItem->setSizeHint(QSize(246,60));
    newItem->setTextAlignment(Qt::AlignVCenter);

    customItem->setItemText(text);
    customItem->setItemIcon(iconPath);
    ui->infoListWidget->addItem(newItem);
    ui->infoListWidget->setItemWidget(newItem , customItem);

    ui->infoListWidget->setGridSize(QSize(246,84));
    return customItem;
}

void kiranSystemInformation::changeWidgetWhenItemClicked(QListWidgetItem * currentItem)
{
    int itemNum = ui->infoListWidget->row(currentItem);
    if(itemNum == itemSystemInfo)
    {
        ui->stackedWidget->setCurrentWidget(ui->page_system_info);
        systemInfomationItem->setItemArrow(true);
        hardwareInformationItem->setItemArrow(false);
    }
    else if(itemNum == itemHardwareInfo)
    {
        ui->stackedWidget->setCurrentWidget(ui->page_hardware_info);
        hardwareInformationItem->setItemArrow(true);
        systemInfomationItem->setItemArrow(false);
    }


}
