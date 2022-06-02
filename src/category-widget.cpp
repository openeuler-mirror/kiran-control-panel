/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */


#include "category-widget.h"
#include "plugin-manager.h"
#include "ui_category-widget.h"

#include <qt5-log-i.h>
#include <QListWidgetItem>
#include <QPainter>
#include <QPropertyAnimation>
#include <QtMath>

#define ROLE_CATEGORY_INDEX Qt::UserRole

CategoryWidget::CategoryWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::CategoryWidget)
{
    ui->setupUi(this);
    init();
}

CategoryWidget::~CategoryWidget()
{
    delete ui;
}

void CategoryWidget::init()
{
    setAttribute(Qt::WA_Hover, true);

    loadCategory();

    m_categorySliderAnimation = new QPropertyAnimation(this, "geometry");
    connect(m_categorySliderAnimation, &QPropertyAnimation::finished, [this]() {
        //由图标+文本显示模式->图标显示模式时等动画完成之后再绘制阴影
        if (ui->categorys->isIconMode())
        {
            ui->categorys->setTextShow(false);
            m_showShadow = false;
            update();
        }
    });

    this->layout()->setContentsMargins(0, 0, m_shadowWidth, 0);
    resize(iconModeWidth(), height());

    connect(ui->categorys, &CategoryListWidget::currentItemChanged, [this](QListWidgetItem *current, QListWidgetItem *prev) {
        int curIndex = ui->categorys->row(current);
        int prevIndex = ui->categorys->row(prev);
        emit sigCurrentCategoryChanged(curIndex, prevIndex);
    });
}

void CategoryWidget::paintEvent(QPaintEvent *event)
{
    if (m_showShadow)
    {
        QPainter painter(this);
        drawShadow(painter);
    }

    QWidget::paintEvent(event);
}

void CategoryWidget::setIconMode(bool iconMode)
{
    ui->categorys->setIconMode(iconMode);
    m_categorySliderAnimation->setDuration(iconMode ? 100 : 300);
    m_categorySliderAnimation->setStartValue(QRect(0, 0, width(), this->height()));
    m_categorySliderAnimation->setEndValue(QRect(0, 0, iconMode ? iconModeWidth() : textModeWd(), this->height()));
    m_categorySliderAnimation->setEasingCurve(iconMode ? QEasingCurve::InExpo : QEasingCurve::OutExpo);
    m_categorySliderAnimation->start();
    ///由图标显示模式->图标+文本显示模式时先显示文本，绘制阴影
    ///由图标+文本显示模式->图标显示模式时等动画完成之后再绘制阴影
    if (!iconMode)
    {
        ui->categorys->setTextShow(true);
        m_showShadow = true;
        update();
    }
}

int CategoryWidget::iconModeWidth()
{
    return 96 + m_shadowWidth + 1;  //加1个像素的右边距.
}

int CategoryWidget::textModeWd()
{
    return 252;
}

void CategoryWidget::drawShadow(QPainter &painter)
{
    painter.save();

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    QRectF rect(10, 10, this->width() - 20, this->height() - 20);
    path.addRoundRect(rect, 8, 8);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for (int i = 0; i < 10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10 - i, 10 - i, this->width() - (10 - i) * 2, this->height() - (10 - i) * 2);
        color.setAlpha(150 - qSqrt(i) * 50);
        painter.setPen(color);
        painter.drawPath(path);
    }
    painter.restore();
}

bool CategoryWidget::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::HoverEnter:
        setIconMode(false);
        break;
    case QEvent::HoverLeave:
        setIconMode(true);
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void CategoryWidget::loadCategory()
{
    ui->categorys->clear();
    QListWidgetItem *categoryItem = nullptr;
    CategoryListWidgetItemWidget *categoryItemWidget = nullptr;
    auto categorys = CPanelPluginManager::getInstance()->getCategorys();
    for (int i = 0; i < categorys.count(); i++)
    {
        auto category = categorys.at(i);
        auto categoryInfo = category->getCategoryDesktopInfo();

        categoryItem = new QListWidgetItem(ui->categorys);
        categoryItem->setData(ROLE_CATEGORY_INDEX, i);
        categoryItem->setSizeHint(QSize(CategoryListWidgetItemWidget::iconModeWd(), CategoryListWidgetItemWidget::heightInt()));
        ui->categorys->addItem(categoryItem);

        categoryItemWidget = new CategoryListWidgetItemWidget;
        categoryItemWidget->setText(categoryInfo.name);
        categoryItemWidget->setIcon(categoryInfo.icon);
        categoryItemWidget->setToolTip(categoryInfo.comment);
        ui->categorys->setItemWidget(categoryItem, categoryItemWidget);

        KLOG_INFO() << "add category " << categoryInfo.categoryName << "to category listwidget";
    }
}

void CategoryWidget::setCurrentCategory(int index)
{
    auto item = ui->categorys->item(index);
    ui->categorys->setCurrentItem(item);
}