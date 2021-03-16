/**
 * @file kiran-system-information.cpp
 * @brief 系统信息页主界面
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "kiran-system-information.h"
#include "ui_kiran-system-information.h"
#include "system-information-widget.h"
#include "general-functions-class.h"

#include <QPainter>
#include <QStackedWidget>
#include <QDesktopWidget>
#include <QDebug>
#include <QBoxLayout>
#include <QScroller>

kiranSystemInformation::kiranSystemInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::kiranSystemInformation)
{
    ui->setupUi(this);
    initUI();
    QSize size = ui->widget_page->sizeHint();
    qInfo() << "widget page size hint = " << size.width() << "," << size.height() << endl;

    QSize size_scroll = ui->scrollArea->sizeHint();
    qInfo() << "widget scroll size hint = " << size_scroll.width() << "," << size_scroll.height() << endl;

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
    qInfo() << "hardwareInformationItem: " <<  hardwareInformationItem->width() << endl;

    QScroller::grabGesture(ui->infoListWidget,QScroller::LeftMouseButtonGesture);
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

void kiranSystemInformation::resizeEvent(QResizeEvent *event)
{
    qInfo() << "list width: " << ui->infoListWidget->width() << " list height: " << ui->infoListWidget->height() << endl;
    qInfo() << "page width: " << ui->widget_page->width() << " page height: " << ui->widget_page->height() << endl;
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

//void kiranSystemInformation::paintEvent(QPaintEvent *)
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}
