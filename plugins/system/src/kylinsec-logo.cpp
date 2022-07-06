//
// Created by liuxinhao on 2022/5/30.
//

#include "kylinsec-logo.h"

#include <QDate>
#include <QPainter>

#define SYSTEM_LOGO "KylinSec OS"

KylinsecLogo::KylinsecLogo(QWidget *parent)
{
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
