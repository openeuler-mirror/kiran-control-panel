/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "category-listwidget-item.h"
#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <qt5-log-i.h>

CategoryListWidgetItemWidget::CategoryListWidgetItemWidget(QWidget *parent) : QWidget(parent), m_statusType(QEvent::None),m_showText(false)
{
    setAttribute(Qt::WA_Hover,true);
    installEventFilter(this);
    setStyleSheet("QToolTip{color:#000000;border: 0px solid #ffffff;background:#b3b3b3;}");
    m_iconRect = QRect(cClassItemWgtLRPadding, cClassItemWgtTBPadding, cClassItemIconWd, cClassItemIconWd);
    m_textRect = QRect(cClassItemWgtLRPadding+cClassItemIconWd+cClassItemSpace, cClassItemWgtTBPadding, cClassItemTextWd, cClassItemIconWd);
}
/*!
 * \brief CategoryListWidgetItemWidget::paintEvent
 * 按钮的图片和文字都是绘制的,间距通过预先规定的两个rect实现.
 */
void CategoryListWidgetItemWidget::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPen pen;
    pen.setWidth(1);
    pen.setColor("#ffffff");
    painter.setPen(pen);
    painter.drawPixmap(m_iconRect, m_icon);
    if(m_showText) painter.drawText(m_textRect, Qt::TextWrapAnywhere|Qt::AlignLeft|Qt::AlignVCenter, m_text);

    QWidget::paintEvent(ev);
}
/*!
 * \brief CategoryListWidgetItemWidget::pixmapToRound 图片圆角化.此函数没有用到,暂时保留.
 * \param src
 * \param size
 * \param radius
 * \return
 */
QPixmap CategoryListWidgetItemWidget::pixmapToRound(const QPixmap &src, const QSize &size, int radius)
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

bool CategoryListWidgetItemWidget::eventFilter(QObject * obj, QEvent * event)
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

void CategoryListWidgetItemWidget::setIcon(const QString &icon)
{
    m_icon =  QPixmap(icon);//pixmapToRound(QPixmap(icon), m_iconRect.size(), 15);
}

void CategoryListWidgetItemWidget::setTextVisible(const bool &visible)
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

void CategoryListWidgetItemWidget::setText(const QString &text)
{
    m_text = text;
}

int CategoryListWidgetItemWidget::textModeWd()
{
    return 2*cClassItemWgtLRPadding + cClassItemSpace + cClassItemIconWd + cClassItemTextWd;
}

int CategoryListWidgetItemWidget::iconModeWd()
{
    return 2*cClassItemWgtLRPadding + cClassItemIconWd;
}

int CategoryListWidgetItemWidget::heightInt()
{
    return 2*cClassItemWgtTBPadding+cClassItemIconWd;
}

