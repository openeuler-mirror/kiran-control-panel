/**
 * @file information-list-item.cpp
 * @brief 系统信息列表
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "information-list-item.h"
#include "ui_information-list-item.h"
#include <QStyleOption>
#include <QPainter>

InformationListItem::InformationListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InformationListItem)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_StyledBackground);
    setAttribute(Qt::WA_StyleSheet);
}

InformationListItem::~InformationListItem()
{
    delete ui;
}

void InformationListItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/**
 * @brief setItemText 设置左边列表项的文字
 * @param text 要设置的文字
 */
void InformationListItem::setItemText(const QString &text)
{
    ui->label_list_text->setText(text);
}

/**
 * @brief setItemIcon 设置左边列表项的图标
 * @param iconPath 图标路径
 */
void InformationListItem::setItemIcon(const QString &iconPath)
{
    ui->label_icon_info->setFixedSize(16,16);
    ui->label_icon_info->setStyleSheet(QString("QLabel#label_icon_info{border-image:url(%1)};").arg(iconPath));
}

void InformationListItem::setItemArrow(bool isSelected)
{
    if(isSelected)
        ui->label_icon_arrow->setStyleSheet("QLabel#label_icon_arrow{border-image:url(:/images/arrow.svg)};");
    else
        ui->label_icon_arrow->setStyleSheet("QLabel#label_icon_arrow{border-image:none");
}

QSize InformationListItem::sizeHint() const
{
    QSize sizeHint = QSize(246,60);
    return sizeHint;
}
