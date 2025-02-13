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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#include "kiran-time-zone-item.h"
#include "ui_kiran-time-zone-item.h"

#include <palette.h>

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

using namespace Kiran::Theme;
KiranTimeZoneItem::KiranTimeZoneItem(const ZoneInfo zoneInfo, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::KiranTimeZoneItem),
      m_searchHighLightColor("#43a3f2"),
      m_isSelected(false),
      m_isResultTip(false)
{
    ui->setupUi(this);

    m_selectedIcon = QIcon(":/kiran-control-panel/images/indicator-selected.png");

    setAttribute(Qt::WA_Hover);
    setFocusPolicy(Qt::TabFocus);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setFixedHeight(40);

    m_city = zoneInfo.zone_city;
    m_zoneID = zoneInfo.zone_id;

    int hour = qAbs(zoneInfo.zone_utc) / 3600;
    int minute = (qAbs(zoneInfo.zone_utc) - hour * 3600) / 60;
    m_displayUTC = QString("UTC%1%2:%3").arg(zoneInfo.zone_utc >= 0 ? "+" : "-").arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0'));

    ui->label->setContentsMargins(10, 0, 0, 0);
    ui->label->setText(QString("(%1) %2").arg(m_displayUTC).arg(m_city));
}

KiranTimeZoneItem::KiranTimeZoneItem(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::KiranTimeZoneItem),
                                                        m_searchHighLightColor("#43a3f2"),
                                                        m_isSelected(false),
                                                        m_isResultTip(true)
{
    ui->setupUi(this);

    m_selectedIcon = QIcon::fromTheme("ksvg-selected");

    setAttribute(Qt::WA_Hover);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(40);

    ui->label->setContentsMargins(10, 0, 0, 0);
    ui->label->setText(QString("<font color=\"gray\">%1</font>").arg(tr("No search results, please search again...")));
}

KiranTimeZoneItem::~KiranTimeZoneItem()
{
    delete ui;
}

QString KiranTimeZoneItem::getTimeZoneID()
{
    return m_zoneID;
}

QColor KiranTimeZoneItem::searchHighLightColor() const
{
    return m_searchHighLightColor;
}

bool KiranTimeZoneItem::isSelected() const
{
    return m_isSelected;
}

void KiranTimeZoneItem::setHeightLightKeyword(const QString &keyword)
{
    if (m_isResultTip)
    {
        return;
    }
    QString city = m_city;
    QString hightLightKeyword = QString("<font color=\"%1\">%2</font>").arg(m_searchHighLightColor.name()).arg(keyword);
    city = city.replace(keyword, hightLightKeyword);
    ui->label->setText(QString("(%1) %3").arg(m_displayUTC).arg(city));
    m_keyword = keyword;
}

void KiranTimeZoneItem::unsetHeightLightKeyword()
{
    if (m_isResultTip)
    {
        return;
    }

    ui->label->setText(QString("(%1) %3").arg(m_displayUTC).arg(m_city));
}

void KiranTimeZoneItem::setSearchHighLightColor(QColor searchHighLightColor)
{
    m_searchHighLightColor = searchHighLightColor;
}

void KiranTimeZoneItem::seletedZoneInfoChanged(const QString &zoneID)
{
    if (m_isResultTip)
    {
        return;
    }

    if (zoneID == m_zoneID)
    {
        setisSelected(true);
    }
    else
    {
        setisSelected(false);
    }
}

void KiranTimeZoneItem::setisSelected(bool isSelected)
{
    if (m_isSelected == isSelected)
        return;
    m_isSelected = isSelected;

    if (m_isSelected)
    {  /// 选中状态，取消掉关键词高亮
        unsetHeightLightKeyword();
        ui->icon->setIcon(m_selectedIcon);
    }
    else
    {  /// 取消选中，若存在关键词则设置关键词高亮
        setHeightLightKeyword(m_keyword);
        ui->icon->setIcon(QIcon());
    }
    emit isSelectedChanged(m_isSelected);
}

void KiranTimeZoneItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_isResultTip)
    {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void KiranTimeZoneItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);

    auto kiranPalette = DEFAULT_PALETTE();

    bool hover = opt.state & QStyle::State_MouseOver;

    QColor background = Qt::transparent, border;
    if (hover)
    {
        background = kiranPalette->getColor(Palette::MOUSE_OVER, Palette::WIDGET);
    }
    border = kiranPalette->getColor(Palette::ACTIVE,
                                    Palette::BORDER);

    QPainter p(this);
    p.fillRect(opt.rect, background);

    QPen pen;
    pen.setColor(border);
    p.setPen(pen);
    p.drawLine(opt.rect.topLeft(), opt.rect.topRight());

    QWidget::paintEvent(event);
}
