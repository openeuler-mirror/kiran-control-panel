/**
 * @file kiran-system-information.cpp
 * @brief 系统信息页主界面
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "kiran-system-information.h"
#include "ui_kiran-system-information.h"

#include <QPainter>
#include <QStackedWidget>

kiranSystemInformation::kiranSystemInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::kiranSystemInformation)
{
    ui->setupUi(this);
    addPagesToStackWidget();
    initUI();
}

kiranSystemInformation::~kiranSystemInformation()
{
    delete ui;
}

void kiranSystemInformation::initUI()
{
    /*创建左侧列表*/
    QString systemInfomationIcon = ":/images/data/system-information.svg";
    QString hardwareInformationIcon = ":/images/data/hardware-information.svg";
    systemInfomationItem = createInformationItem(QString(tr("System Information")) , systemInfomationIcon);
    hardwareInformationItem = createInformationItem(QString(tr("Hardware Information")) , hardwareInformationIcon);
    hardwareInformationItem->setStyleSheet("#hardwareInformationItem{margin-top:24px};");
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

    return customItem;
}

/**
 * @brief addPagesToStackWidget 将系统信息页和硬件信息页添加到QStackWidget控件中，然后添加至界面右边滑动区域
 */
void kiranSystemInformation::addPagesToStackWidget()
{
    /*获取右侧信息页滑动区域*/
    QWidget *hardwareInfoWidget = new HardwareInformationWidget;
    hardwareInfoWidget->setStyleSheet("#hardwareInfoWidget{background-color: #222222;}");

    QStackedWidget *stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(hardwareInfoWidget);

    ui->scrollArea->setWidget(stackedWidget);
}

//void kiranSystemInformation::paintEvent(QPaintEvent *)
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}
