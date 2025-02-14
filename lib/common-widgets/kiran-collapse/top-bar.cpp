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

#include "top-bar.h"
#include <qt5-log-i.h>
#include "logging-category.h"
#include "ui_top-bar.h"

#include <kiran-switch-button.h>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include "QtSvg/QSvgRenderer"

TopBar::TopBar(QWidget* parent) : QWidget(parent),
                                  ui(new Ui::TopBar)
{
    ui->setupUi(this);
    init();
}

TopBar::~TopBar()
{
    delete ui;
}

void TopBar::setTitle(const QString& title)
{
    m_title = title;
    ui->title->setText(m_title);
}

void TopBar::init()
{
    setContentsMargins(m_topBarMarginLeft,
                       m_topBarMarginTop,
                       m_topBarMarginRight,
                       m_topBarMarginBottom);
    ui->horizontalLayout->setSpacing(m_spacing);
    this->setFixedHeight(m_height);
    this->refreshFlagIcon(true);
}

void TopBar::refreshFlagIcon(bool isExpanded)
{
    if (isExpanded)
    {
        ui->flag->setIcon(rotateFlagIcon(90));
    }
    else
    {
        ui->flag->setIcon(rotateFlagIcon(-90));
    }
}

void TopBar::addWidget(QWidget* widget)
{
    ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, widget);
}

void TopBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (this->geometry().contains(this->mapFromGlobal(event->globalPos())))
    {
        emit clickedBar();
    }
}

void TopBar::setTopBarMargin(int left, int top, int right, int bottom)
{
    m_topBarMarginLeft = left;
    m_topBarMarginTop = top;
    m_topBarMarginRight = right;
    m_topBarMarginBottom = bottom;
    setContentsMargins(m_topBarMarginLeft,
                       m_topBarMarginTop,
                       m_topBarMarginRight,
                       m_topBarMarginBottom);
}
void TopBar::setTopBarSpacing(int spacing)
{
    m_spacing = spacing;
    ui->horizontalLayout->setSpacing(m_spacing);
}

QIcon TopBar::rotateFlagIcon(qreal angle)
{
    auto icon = QIcon::fromTheme("ksvg-arrow");
    auto pixmap = icon.pixmap(QSize(16, 16));
    // 创建旋转变换
    QTransform transform;
    transform.rotate(angle);

    // 应用旋转
    QPixmap rotatedPixmap = pixmap.transformed(transform, Qt::SmoothTransformation);

    // 将旋转后的 QPixmap 转换为 QIcon
    return QIcon(rotatedPixmap);
}
