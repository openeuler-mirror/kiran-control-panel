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

#include "kiran-collapse.h"
#include "ui_kiran-collapse.h"

#include <palette.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOption>

using namespace Kiran::Theme;

KiranCollapse::KiranCollapse(QWidget *parent)
    : KiranCollapse(false, "", nullptr, parent)
{
}

KiranCollapse::KiranCollapse(bool defaultIsExpand, const QString &title,
                             QWidget *expansionSpaceWidget, QWidget *parent)
    : QWidget(parent), ui(new Ui::KiranCollapse), m_isExpanded(defaultIsExpand), m_topBarTitle(title), m_esWidget(expansionSpaceWidget)
{
    init();
}

KiranCollapse::~KiranCollapse()
{
    delete ui;
}

void KiranCollapse::addExpansionSpaceWidget(QWidget *widget)
{
    auto layout = ui->expansionSpaceContainer;
    layout->addWidget(widget);
}

void KiranCollapse::delExpansionSpaceWidget(int index)
{
    if (ui->expansionSpaceContainer->count() == 0)
    {
        return;
    }
    auto item = ui->expansionSpaceContainer->takeAt(index);
    if (item)
    {
        QWidget *widget = item->widget();
        delete widget;
        delete item;
    }
}

void KiranCollapse::delExpansionSpaceWidget(const QString &widgetName)
{
    // 通过对象名称查找和删除指定的widget
    QList<QWidget *> widgetsToRemove = ui->expansionSpaceContainer->findChildren<QWidget *>(widgetName);
    for (QWidget *widget : widgetsToRemove)
    {
        delExpansionSpaceWidget(ui->expansionSpaceContainer->indexOf(widget));
    }
}

void KiranCollapse::delAllExpansionSpaceWidget()
{
    while (ui->expansionSpaceContainer->count() != 0)
    {
        QLayoutItem *item = ui->expansionSpaceContainer->takeAt(0);
        if (item)
        {
            delete item->widget();
            delete item;
        }
    }
}

void KiranCollapse::expand()
{
    if (m_isExpanded || m_animationForES->state() == QAbstractAnimation::Running)
    {
        return;
    }
    m_animationForES->setEasingCurve(QEasingCurve::OutCubic);
    m_animationForES->setStartValue(ui->expansionSpace->height());
    m_animationForES->setEndValue(m_maximumExpansionSpaceHeight);
    m_animationForES->start();

    m_isExpanded = true;
    ui->topBar->refreshFlagIcon(m_isExpanded);
    emit expandSpaceExpanded();
}

void KiranCollapse::collapse()
{
    if (!m_isExpanded || m_animationForES->state() == QAbstractAnimation::Running)
    {
        return;
    }
    m_animationForES->setEasingCurve(QEasingCurve::InCubic);
    m_animationForES->setStartValue(m_maximumExpansionSpaceHeight);
    m_animationForES->setEndValue(0);
    m_animationForES->start();
    m_isExpanded = false;
    ui->topBar->refreshFlagIcon(m_isExpanded);
    emit expandSpaceCollapsed();
}

void KiranCollapse::changeExpansionState()
{
    if (m_isExpanded)
    {
        collapse();
    }
    else
    {
        expand();
    }
}

void KiranCollapse::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    QStyle::State state;

    opt.initFrom(this);
    state = opt.state;

    QPainterPath painterPath;
    QRectF qRect = opt.rect;
    qRect.adjust(0.5, 0.5, -0.5, -0.5);
    painterPath.addRoundedRect(qRect, m_radius, m_radius);

    using namespace Kiran::Theme;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto kiranPalette = DEFAULT_PALETTE();
    if (m_drawBackground)
    {
        QColor backgroundColor;
        backgroundColor = kiranPalette->getColor(state, Palette::WIDGET);
        painter.fillPath(painterPath, backgroundColor);
    }

    QWidget::paintEvent(event);
}

void KiranCollapse::init()
{
    ui->setupUi(this);
    ui->expansionSpace->setAttribute(Qt::WA_StyledBackground);
    ui->expansionSpaceContainer->setSpacing(12);

    setIsExpand(m_isExpanded);
    ui->expansionSpaceContainer->setContentsMargins(m_expansionMarginLeft,
                                                    m_expansionMarginTop,
                                                    m_expansionMarginRight,
                                                    m_expansionMarginBottom);
    ui->topBar->setTitle(m_topBarTitle);
    // 若为扩展区控件为空，则不添加
    if (m_esWidget)
    {
        ui->expansionSpaceContainer->addWidget(m_esWidget);
    }
    m_animationForES = new QPropertyAnimation(ui->expansionSpace, "maximumHeight", this);
    m_animationForES->setDuration(200);
    connect(ui->topBar, &TopBar::clickedBar, this, &KiranCollapse::changeExpansionState);
}

bool KiranCollapse::getIsExpand() const
{
    return m_isExpanded;
}

void KiranCollapse::setIsExpand(bool isExpanded)
{
    m_isExpanded = isExpanded;
    // 根据展开/折叠设置最大高度
    ui->expansionSpace->setMaximumHeight(m_isExpanded ? m_maximumExpansionSpaceHeight : 0);
    ui->topBar->refreshFlagIcon(m_isExpanded);
}

void KiranCollapse::addTopBarWidget(QWidget *widget)
{
    ui->topBar->addWidget(widget);
}

void KiranCollapse::setTitle(const QString &title)
{
    ui->topBar->setTitle(title);
}

void KiranCollapse::setTobBarFixedHeight(int height)
{
    ui->topBar->setFixedHeight(height);
}

void KiranCollapse::setMaximumExpansionHeight(int maxExpandHeight)
{
    m_maximumExpansionSpaceHeight = maxExpandHeight;
    // 缓存设置最大高度之前的原始高度
    int curHeight = ui->expansionSpace->height();
    ui->expansionSpace->setMaximumHeight(maxExpandHeight);
    // 如果设置最大高度时，处于折叠态则依然保持设置前高度 (防止折叠态是设置该值会导致扩展区展开)
    if (!m_isExpanded)
    {
        ui->expansionSpace->setFixedHeight(curHeight);
    }
}
void KiranCollapse::setExpansionMargin(int left, int top, int right, int bottom)
{
    m_expansionMarginLeft = left;
    m_expansionMarginTop = top;
    m_expansionMarginRight = right;
    m_expansionMarginBottom = bottom;
    ui->expansionSpaceContainer->setContentsMargins(m_expansionMarginLeft,
                                                    m_expansionMarginTop,
                                                    m_expansionMarginRight,
                                                    m_expansionMarginBottom);
}

QMargins KiranCollapse::expansionMargin()
{
    return QMargins(m_expansionMarginLeft,
                    m_expansionMarginTop,
                    m_expansionMarginRight,
                    m_expansionMarginBottom);
}

void KiranCollapse::setTopBarMargin(int left, int top, int right, int bottom)
{
    ui->topBar->setTopBarMargin(left, top, right, bottom);
}
void KiranCollapse::setTopBarSpacing(int spacing)
{
    ui->topBar->setTopBarSpacing(spacing);
}
void KiranCollapse::setExpand()
{
    expand();
}
void KiranCollapse::setCollapse()
{
    collapse();
}
