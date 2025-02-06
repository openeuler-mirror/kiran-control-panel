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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */
#include "setting-brief-widget.h"
#include "ui_setting-brief-widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <palette.h>
#include <style-helper.h>
#include <QPainterPath>

using namespace Kiran::Theme;

SettingBriefWidget::SettingBriefWidget(QString title, int type, QWidget *parent) : QWidget(parent),
                                                                                   ui(new Ui::SettingBriefWidget)
{
    setAccessibleName("SettingBriefWidget");
    ui->setupUi(this);
    initUI(title);
    setAttribute(Qt::WA_Hover);
    m_wallpaperType = type;
}

SettingBriefWidget::~SettingBriefWidget()
{
    delete ui;
}

void SettingBriefWidget::setName(QString name)
{
    ui->label_name->setText(name);
}

void SettingBriefWidget::initUI(QString title)
{
    ui->label_text->setText(title);
    ui->label_arrow->setFixedSize(16, 16);
    updateThemeArrowIcon();
    connect(DEFAULT_PALETTE(), &Palette::baseColorsChanged, this, &SettingBriefWidget::updateThemeArrowIcon);
}

void SettingBriefWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void SettingBriefWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);

    Palette::ColorGroup colorState = Palette::ColorGroup::ACTIVE;
    if (!(opt.state & QStyle::State_Enabled))
    {
        colorState = Palette::ColorGroup::DISABLED;
    }
    else if (opt.state & QStyle::State_Sunken)
    {
        colorState = Palette::ColorGroup::SUNKEN;
    }
    else if (opt.state & QStyle::State_MouseOver)
    {
        colorState = Palette::ColorGroup::MOUSE_OVER;
    }

    auto background = DEFAULT_PALETTE()->getColor(colorState, Palette::ColorRole::WIDGET);

    QPainter p(this);
    QPainterPath painterPath;
    painterPath.addRoundedRect(opt.rect, 6, 6);

    p.fillPath(painterPath, background);
}

void SettingBriefWidget::updateThemeArrowIcon()
{
    QPixmap pixmap(":/kcp-appearance/images/select.svg");

    if (DEFAULT_STYLE_HELPER()->paletteType() != PaletteType::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }

    ui->label_arrow->setPixmap(pixmap);
}
