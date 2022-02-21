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

#include "kiran-system-information.h"
#include <kiran-log/qt5-log-i.h>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QPainter>
#include <QScroller>
#include <QStackedWidget>
#include "ui_kiran-system-information.h"

kiranSystemInformation::kiranSystemInformation(QWidget* parent) : QWidget(parent),
                                                                  ui(new Ui::kiranSystemInformation)
{
    ui->setupUi(this);
    initUI();

    //connect(ui->infoListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(changeWidgetWhenItemClicked(QListWidgetItem*)));
}

kiranSystemInformation::~kiranSystemInformation()
{
    delete ui;
}

QSize kiranSystemInformation::sizeHint() const
{
    /*根据系统分辨率设置窗口大小*/
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);

    KLOG_INFO() << screenGeometry.width() << screenGeometry.height();
    QSize windowSize;
    if (screenGeometry.height() >= 776 && screenGeometry.width() >= 980)  //能显示全
    {
        windowSize = QSize(980, 776);
    }
    else
    {
        windowSize = QSize(screenGeometry.width(), screenGeometry.height());
    }

    return windowSize;
}

void kiranSystemInformation::initUI()
{
    /*创建左侧列表*/
    QString licenseInfomationIcon = ":/images/system-information.svg";
    licenseInfoWidget = createInformationItem(QString(tr("License Information")), licenseInfomationIcon);
    ui->infoListWidget->setCurrentRow(0);
}

/**
 * @brief  创建左侧自定义信息列表项
 * @param  text 列表项的名字
 * @return InfomationListItem 返回设计好的信息列表项
 */
InformationListItem* kiranSystemInformation::createInformationItem(const QString text, const QString iconPath)
{
    QListWidgetItem* newItem = nullptr;
    InformationListItem* customItem = nullptr;

    newItem = new QListWidgetItem(ui->infoListWidget);
    customItem = new InformationListItem(ui->infoListWidget);

    newItem->setSizeHint(QSize(246, 60));
    newItem->setTextAlignment(Qt::AlignVCenter);

    customItem->setItemText(text);
    customItem->setItemIcon(iconPath);
    ui->infoListWidget->addItem(newItem);
    ui->infoListWidget->setItemWidget(newItem, customItem);

    ui->infoListWidget->setGridSize(QSize(246, 84));
    return customItem;
}

void kiranSystemInformation::changeWidgetWhenItemClicked(QListWidgetItem* currentItem)
{
    //    int itemNum = ui->infoListWidget->row(currentItem);
    //    if (itemNum == itemSystemInfo)
    //    {
    //        ui->stackedWidget->setCurrentWidget(ui->page_system_info);
    //        systemInfomationItem->setItemArrow(true);
    //        hardwareInformationItem->setItemArrow(false);
    //    }
    //    else if (itemNum == itemHardwareInfo)
    //    {
    //        ui->stackedWidget->setCurrentWidget(ui->page_hardware_info);
    //        hardwareInformationItem->setItemArrow(true);
    //        systemInfomationItem->setItemArrow(false);
    //    }
}
