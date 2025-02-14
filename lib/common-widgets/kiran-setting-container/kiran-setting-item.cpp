/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * ks-sc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#include "kiran-setting-item.h"
#include <kiran-label.h>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include "auxiliary.h"
#include "kiran-switch-button.h"

using namespace Kiran::Theme;

KiranSettingItem::KiranSettingItem(QWidget* parent)
    : KiranFrame(parent)
{
    initUI();
}

KiranSettingItem::~KiranSettingItem()
{
}

void KiranSettingItem::setUserData(QVariant userData)
{
    m_userData = userData;
}

void KiranSettingItem::setText(const QString& text)
{
    m_label->setText(text);
}

QString KiranSettingItem::getText()
{
    return m_label->text();
}

void KiranSettingItem::setLeftButtonVisible(bool visible, const QIcon& icon)
{
    m_leftButton->setVisible(visible);
    m_leftButton->setIcon(icon);
}

void KiranSettingItem::setRightButtonVisible(bool visible, const QIcon& icon)
{
    m_rightButton->setVisible(visible);
    m_rightButton->setIcon(icon);
}

void KiranSettingItem::setSwitcherVisible(bool visible)
{
    m_switcher->setVisible(visible);
}

void KiranSettingItem::setSwitcherChecked(bool checked)
{
    m_switcher->setChecked(checked);
}

void KiranSettingItem::setClickable(bool clickable)
{
    if (m_clickable == clickable)
    {
        return;
    }

    m_clickable = clickable;
    setAttribute(Qt::WA_Hover, m_clickable);
}

void KiranSettingItem::initUI()
{
    setDrawBroder(false);
    setFixedHeight(36);

    auto layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(6);

    m_leftButton = new QPushButton(this);
    m_leftButton->setFixedSize(16, 16);
    m_leftButton->setFlat(true);
    m_leftButton->setVisible(true);
    layout->addWidget(m_leftButton);
    connect(m_leftButton, &QPushButton::clicked, this, [this]()
            { emit leftButtonClicked(m_userData); });

    m_label = new KiranLabel(this);
    m_label->setAlignment(Qt::AlignVCenter);
    m_label->setElideMode(Qt::ElideRight);
    layout->addWidget(m_label, 0, Qt::AlignVCenter);

    layout->addStretch();

    m_switcher = new KiranSwitchButton(this);
    m_switcher->setVisible(false);
    layout->addWidget(m_switcher, 0, Qt::AlignVCenter);
    connect(m_switcher, &KiranSwitchButton::toggled, this, [this](bool checked)
            { emit switchButtonToggled(m_userData, checked); });

    m_rightButton = new QPushButton(this);
    m_rightButton->setFixedSize(16, 16);
    m_rightButton->setFlat(true);
    m_rightButton->setVisible(false);
    layout->addWidget(m_rightButton);
    connect(m_rightButton, &QPushButton::clicked, this, [this]()
            { emit rightButtonClicked(m_userData); });
}

void KiranSettingItem::mousePressEvent(QMouseEvent* event)
{
    if (m_clickable)
    {
        m_clicked = true;
    }

    KiranFrame::mousePressEvent(event);
}

void KiranSettingItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_clickable && m_clicked)
    {
        emit clicked();
    }
    m_clicked = false;

    KiranFrame::mouseReleaseEvent(event);
}
