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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#include "kylinsec-logo.h"

#include <QDate>
#include <QPainter>

#define SYSTEM_LOGO "KylinSec OS"

KylinsecLogo::KylinsecLogo(QWidget *parent)
{
    setAccessibleName("KylinsecLogo");
}

KylinsecLogo::~KylinsecLogo()
{
}

void KylinsecLogo::paintEvent(QPaintEvent *event)
{
    QDate currentDate = QDate::currentDate();
    QString date = currentDate.toString("yyyy-MM-dd");
    QString year = date.left(4);
    QString copyright = QString(tr("Copyright ©")) + QString("%1 ").arg(year) + QString(tr("KylinSec. All rights reserved."));

    QPainter painter(this);
    QFont font = QFont("Noto Sans CJK SC regular", 46);
    QRect drawRecLogo = QRect( 24, 16, this->width(), 157 - 16);

    painter.setPen(QColor(46, 179, 255));  //#2eb3FF
    painter.setFont(font);
    painter.drawText(drawRecLogo, SYSTEM_LOGO);

    QFontMetrics fm = painter.fontMetrics();
    int heightText = fm.height();

    int offsetHeight = heightText + 5 + 16;
    QRect drawRecCopyright = QRect(24, offsetHeight, this->width(), 157 - offsetHeight);
    font.setPointSize(10);
    font.setWeight(QFont::Normal);
    painter.setPen(QColor(145, 145, 145));
    painter.setFont(font);
    painter.drawText(drawRecCopyright, copyright);
}
