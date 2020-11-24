/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-module-class-listwidget-itemwidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QBitmap>
#include <QDebug>

KiranModuleClassListWidgetItemWidget::KiranModuleClassListWidgetItemWidget(QWidget *parent) : QWidget(parent), m_statusType(QEvent::None),m_showText(false), m_checked(false)
{
    setAttribute(Qt::WA_Hover,true);
    installEventFilter(this);
    setStyleSheet("QToolTip{color:#000000;border: 0px solid #ffffff;background:#b3b3b3;}");
    m_iconRect = QRect(cClassItemWgtLRPadding, cClassItemWgtTBPadding, cClassItemIconWd, cClassItemIconWd);
    m_textRect = QRect(cClassItemWgtLRPadding+cClassItemIconWd+cClassItemSpace, cClassItemWgtTBPadding, cClassItemTextWd, cClassItemIconWd);
}
/*!
 * \brief KiranModuleClassListWidgetItemWidget::paintEvent
 * 按钮的图片和文字都是绘制的,间距通过预先规定的两个rect实现.
 */
void KiranModuleClassListWidgetItemWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(1);
    pen.setColor("#ffffff");
    painter.setPen(pen);
    painter.drawPixmap(m_iconRect, m_icon);
    if(m_showText) painter.drawText(m_textRect, Qt::TextWrapAnywhere|Qt::AlignLeft|Qt::AlignVCenter, m_text);
}


QPixmap KiranModuleClassListWidgetItemWidget::pixmapToRound(const QPixmap &src, const QSize &size, int radius)
{
    if (Q_UNLIKELY(src.isNull())) {
        return QPixmap();
    }

    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(0, 0, size.width(), size.height(), radius, radius);

    QPixmap image = src.scaled(size);
    image.setMask(mask);
    return image;
}

void KiranModuleClassListWidgetItemWidget::setChecked(bool checked)
{
    m_checked = checked;
}

bool KiranModuleClassListWidgetItemWidget::eventFilter(QObject * obj, QEvent * event)
{
    if(Q_LIKELY(obj == this))
    {
        switch (event->type()) {
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
            m_statusType = event->type();
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void KiranModuleClassListWidgetItemWidget::setIcon(const QString &icon)
{
    m_icon =  pixmapToRound(QPixmap(icon), m_iconRect.size(), 15);
}

void KiranModuleClassListWidgetItemWidget::setTextVisible(const bool &visible)
{
    m_showText = visible;
    if(visible)
    {
        setFixedSize(textModeWd(), heightInt());
    }
    else
    {
        setFixedSize(iconModeWd(), heightInt());
    }
}

void KiranModuleClassListWidgetItemWidget::setText(const QString &text)
{
    m_text = text;
}

int KiranModuleClassListWidgetItemWidget::textModeWd()
{
    return 2*cClassItemWgtLRPadding + cClassItemSpace + cClassItemIconWd + cClassItemTextWd;
}

int KiranModuleClassListWidgetItemWidget::iconModeWd()
{
    return 2*cClassItemWgtLRPadding + cClassItemIconWd;
}

int KiranModuleClassListWidgetItemWidget::heightInt()
{
    return 2*cClassItemWgtTBPadding+cClassItemIconWd;
}

