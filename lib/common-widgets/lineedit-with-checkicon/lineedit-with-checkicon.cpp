 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
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
