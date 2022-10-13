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

#include "lineedit-with-checkicon.h"

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QStyleOption>
#include <QSvgRenderer>

LineEditWithCheckIcon::LineEditWithCheckIcon(QWidget *parent)
    : KiranIconLineEdit(parent),
      m_verificationStatus(VERIFICATION_NONE)
{
    initUI();
}

LineEditWithCheckIcon::~LineEditWithCheckIcon()
{
}

void LineEditWithCheckIcon::setVerificationStatus(bool isPassed)
{
    QString iconPath = isPassed ? ":/kiran-control-panel/images/success-indicator.svg" : ":/kiran-control-panel/images/error-indicator.svg";
    m_verificationStatus = isPassed ? VERIFICATION_PASSED : VERIFICATION_ERROR;
    setIcon(QIcon(iconPath));
}

void LineEditWithCheckIcon::initUI()
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setIconSize(QSize(16, 16));
    setIconPosition(KiranIconLineEdit::ICON_POSITION_RIGHT);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    connect(this, &QLineEdit::textChanged, [this](const QString &text) {
        ///密码框输入密码不为空的情况下调整字体和字间距
        if (echoMode() == QLineEdit::Password && text.isEmpty())
        {
            setShowPasswordModeStyle(false);
            setNormalLetterSpacing();
        }
        else if (echoMode() == QLineEdit::Password && !text.isEmpty())
        {
            setShowPasswordModeStyle(true);
            setPasswdLetterSpacing();
        }
    });
}

void LineEditWithCheckIcon::setNormalLetterSpacing()
{
    QFont currentFont = font();
    currentFont.setLetterSpacing(QFont::PercentageSpacing, 0);
    setFont(currentFont);
}

void LineEditWithCheckIcon::setPasswdLetterSpacing()
{
    QFont currentFont = font();
    currentFont.setLetterSpacing(QFont::AbsoluteSpacing, 4);
    setFont(currentFont);
}

void LineEditWithCheckIcon::setShowPasswordModeStyle(bool showPasswordModeStyle)
{
    if (m_showPasswordModeStyle == showPasswordModeStyle)
    {
        return;
    }
    m_showPasswordModeStyle = showPasswordModeStyle;
    style()->polish(this);
}

void LineEditWithCheckIcon::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && echoMode() == QLineEdit::Password)
    {
        clear();
    }
    KiranIconLineEdit::keyPressEvent(event);
}

void LineEditWithCheckIcon::focusInEvent(QFocusEvent *event)
{
    resetVerificationStatus();
    KiranIconLineEdit::focusInEvent(event);
}

void LineEditWithCheckIcon::resetVerificationStatus()
{
    m_verificationStatus = VERIFICATION_NONE;
    setIcon(QIcon());
}
