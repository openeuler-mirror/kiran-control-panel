#include "kiran-mode-class-listwidget-item-widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QBitmap>
#include <QDebug>

KiranModeClassListWidgetItemWidget::KiranModeClassListWidgetItemWidget(QWidget *parent) : QWidget(parent), m_statusType(QEvent::None),m_showText(false), m_checked(false)
{
    setAttribute(Qt::WA_Hover,true);
    installEventFilter(this);
    setStyleSheet("QToolTip{color:#000000;border: 0px solid #ffffff;background:#b3b3b3;}");
    setCursor(Qt::OpenHandCursor);
    m_iconRect = QRect(cClassItemWgtLRPadding, cClassItemWgtTBPadding, cClassItemIconWd, cClassItemIconWd);
    m_textRect = QRect(cClassItemWgtLRPadding+cClassItemIconWd+cClassItemSpace, cClassItemWgtTBPadding, cClassItemTextWd, cClassItemIconWd);
}

void KiranModeClassListWidgetItemWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(1);
    pen.setColor("#ffffff");
    painter.setPen(pen);
    painter.drawPixmap(m_iconRect, m_icon);
    if(m_showText) painter.drawText(m_textRect, Qt::TextWrapAnywhere|Qt::AlignCenter, m_text);
}


QPixmap KiranModeClassListWidgetItemWidget::pixmapToRound(const QPixmap &src, const QSize &size, int radius)
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

void KiranModeClassListWidgetItemWidget::setChecked(bool checked)
{
    m_checked = checked;
}

bool KiranModeClassListWidgetItemWidget::eventFilter(QObject * obj, QEvent * event)
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

void KiranModeClassListWidgetItemWidget::setIcon(const QString &icon)
{
    m_icon =  pixmapToRound(QPixmap(icon), m_iconRect.size(), 15);
}

void KiranModeClassListWidgetItemWidget::setTextVisible(const bool &visible)
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

void KiranModeClassListWidgetItemWidget::setText(const QString &text)
{
    m_text = text;
}

int KiranModeClassListWidgetItemWidget::textModeWd()
{
    return 2*cClassItemWgtLRPadding + cClassItemSpace + cClassItemIconWd + cClassItemTextWd;
}

int KiranModeClassListWidgetItemWidget::iconModeWd()
{
    return 2*cClassItemWgtLRPadding + cClassItemIconWd;
}

int KiranModeClassListWidgetItemWidget::heightInt()
{
    return 2*cClassItemWgtTBPadding+cClassItemIconWd;
}

