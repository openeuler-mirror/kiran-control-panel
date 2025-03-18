/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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

#include "user-list-item.h"
#include <kiran-label.h>
#include <QBoxLayout>
#include <QPushButton>
UserListItem::UserListItem(QWidget* parent) : KiranFrame(parent)
{
    initUI();
}

void UserListItem::setName(const QString& name)
{
    m_name = name;
    m_label->setText(name);
}

QString UserListItem::name()
{
    return m_name;
}

void UserListItem::setRightBtnIcon(const QIcon& icon)
{
    m_rightButton->setIcon(icon);
}

void UserListItem::setRightBtnVisible(bool visible)
{
    m_rightButton->setVisible(visible);
}

void UserListItem::setClickable(bool clickable)
{
    if (m_clickable == clickable)
    {
        return;
    }

    m_clickable = clickable;
    setAttribute(Qt::WA_Hover, m_clickable);
}

bool UserListItem::isSelected()
{
    if (m_clickable)
        return m_selected;
    else
        return false;
}

void UserListItem::initUI()
{
    setDrawBroder(false);
    setFixedHeight(36);

    auto layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(6);

    m_label = new KiranLabel(this);
    m_label->setAlignment(Qt::AlignVCenter);
    m_label->setElideMode(Qt::ElideRight);
    layout->addWidget(m_label, 0, Qt::AlignVCenter);

    layout->addStretch();

    m_rightButton = new QPushButton(this);
    m_rightButton->setFixedSize(16, 16);
    m_rightButton->setFlat(true);
    m_rightButton->setVisible(false);
    layout->addWidget(m_rightButton);
    connect(m_rightButton, &QPushButton::clicked, this, [this]()
            { emit rightButtonClicked(); });
}

void UserListItem::mousePressEvent(QMouseEvent* event)
{
    if (m_clickable)
    {
        m_clicked = true;
    }

    KiranFrame::mousePressEvent(event);
}

void UserListItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_clickable && m_clicked)
    {
        m_rightButton->setVisible(!m_rightButton->isVisible());
        m_selected = m_rightButton->isVisible();
    }
    m_clicked = false;

    KiranFrame::mouseReleaseEvent(event);
}
