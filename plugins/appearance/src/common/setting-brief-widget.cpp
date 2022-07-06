/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
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

#include "include.h"
#include "setting-brief-widget.h"
#include "ui_setting-brief-widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <style-palette.h>
#include <QPainterPath>

using namespace Kiran;
//TODO:箭头颜色应当随着主题变更
SettingBriefWidget::SettingBriefWidget(QString title, int type, QWidget *parent) : QWidget(parent),
                                                                         ui(new Ui::SettingBriefWidget)
{
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
    ui->label_arrow->setPixmap(QPixmap(getThemeArrowIcon()));
    connect(StylePalette::instance(),&StylePalette::themeChanged,this,&SettingBriefWidget::updateThemeArrowIcon);
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

    StylePalette::ColorState colorState = StylePalette::Normal;
    if( !(opt.state & QStyle::State_Enabled) )
    {
        colorState = StylePalette::Disabled;
    }
    else if( opt.state & QStyle::State_Sunken )
    {
        colorState = StylePalette::Active;
    }
    else if ( opt.state & QStyle::State_MouseOver )
    {
        colorState = StylePalette::Hover;
    }

    auto kiranPalette = StylePalette::instance();
    auto background = kiranPalette->color(colorState,StylePalette::Widget,StylePalette::Background);

    QPainter p(this);
    QPainterPath painterPath;
    painterPath.addRoundedRect(opt.rect,6,6);

    p.fillPath(painterPath,background);
}

QString SettingBriefWidget::getThemeArrowIcon()
{
    static QMap<Kiran::PaletteType,QString> arrowIcons = {
        {PALETTE_LIGHT,":/kcp-appearance/images/select-black.svg"},
        {PALETTE_DARK,":/kcp-appearance/images/select.svg"}
    };
    return arrowIcons.value(StylePalette::instance()->paletteType());
}

void SettingBriefWidget::updateThemeArrowIcon()
{
    ui->label_arrow->setPixmap(QPixmap(getThemeArrowIcon()));
}
