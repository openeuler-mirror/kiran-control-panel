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
#include <palette.h>
#include <style-helper.h>
#include <QMouseEvent>
#include <QPainter>
#include "QtSvg/QSvgRenderer"
#include "top-bar-flag-pixmap.h"

using namespace Kiran::Theme;
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
    this->refreshFlagPixmap(true);

    connect(DEFAULT_PALETTE(), &Palette::baseColorsChanged, this, [=]()
            {
        // 将 QPixmap 转换为 QImage
        QImage image = ui->flag->pixmap()->toImage();

        // 反转颜色
        for (int y = 0; y < image.height(); ++y)
        {
            for (int x = 0; x < image.width(); ++x)
            {
                QColor color = image.pixelColor(x, y);
                QColor invertedColor = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());
                image.setPixelColor(x, y, invertedColor);
            }
        }

        // 将反转颜色后的 QImage 设置为标志的 pixmap
        ui->flag->setPixmap(QPixmap::fromImage(image)); });
}

void TopBar::setFlagPixmap(const QString& flag_url)
{
    if (flag_url.isEmpty())
    {
        KLOG_WARNING(qLcCommonWidget) << "flag_url is empty!";
        return;
    }
    m_flag = QPixmap(flag_url).scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->flag->setPixmap(m_flag);
}

void TopBar::setFlagPixmap(const QPixmap& pixmap)
{
    m_flag = pixmap;
    ui->flag->setPixmap(m_flag);
}

void TopBar::refreshFlagPixmap(bool isExpanded)
{
    auto styleType = DEFAULT_STYLE_HELPER()->paletteType();
    // clang-format off
    if (isExpanded)
    {
        ui->flag->setPixmap(
            (styleType == PaletteType::PALETTE_DARK) ?
                   FlagPixmap::expansionFlagPixmap().scaled(ui->flag->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation) :
                   FlagPixmap::expansionFlagPixmapDark().scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
        return;
    }
    ui->flag->setPixmap(
        (styleType == PaletteType::PALETTE_DARK) ?
               FlagPixmap::collapseFlagPixmap().scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation) :
               FlagPixmap::collapseFlagPixmapDark().scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    // clang-format on
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
