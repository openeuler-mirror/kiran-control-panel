#include "kiran-mode-class-item-wgt.h"
#include <QMouseEvent>
#include <QPainter>
#include <QBitmap>
#include <QDebug>

KiranModeClassItemWgt::KiranModeClassItemWgt(QWidget *parent) : QWidget(parent), m_statusType(QEvent::None),m_showText(false), m_checked(false)
{
    setAttribute(Qt::WA_Hover,true);
    installEventFilter(this);
    setStyleSheet("QToolTip{color:#000000;border: 0px solid #ffffff;background:#b3b3b3;}");
    setCursor(Qt::OpenHandCursor);
    m_iconRect = QRect(cClassItemPadding, cClassItemPadding, cClassItemIconWd, cClassItemIconWd);
    m_textRect = QRect(cClassItemPadding+cClassItemIconWd+cClassItemSpace, cClassItemPadding, cClassItemTextWd, cClassItemIconWd);
}

void KiranModeClassItemWgt::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect rect = this->rect();
    rect.adjust(1, 1, -1, -1);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect, 10, 10);

    float pixsize = 22;
    QFont font;
    font.setPixelSize(pixsize);
    painter.setFont(font);

    QPen pen;
    pen.setWidth(1);
    pen.setColor("#ffffff");
    painter.setPen(pen);

    QBrush brush;
    if(m_checked)
    {
        brush = QBrush("#2eb3ff");
    }
    else
    {
        switch (m_statusType) {
        case QEvent::HoverEnter://hover
            brush = QBrush(QColor(121, 195, 255));
            break;
        case QEvent::FocusIn://checked
            brush = QBrush("#2eb3ff");
            break;
        default:
            break;
        }
    }

    painter.fillPath(path, brush);
    painter.drawPixmap(m_iconRect, m_icon);
    if(m_showText) painter.drawText(m_textRect, Qt::TextWrapAnywhere|Qt::AlignCenter, m_text);
}


QPixmap KiranModeClassItemWgt::pixmapToRound(const QPixmap &src, const QSize &size, int radius)
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

void KiranModeClassItemWgt::setChecked(bool checked)
{
    m_checked = checked;
}

bool KiranModeClassItemWgt::eventFilter(QObject * obj, QEvent * event)
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

void KiranModeClassItemWgt::setIcon(const QString &icon)
{
    m_icon =  pixmapToRound(QPixmap(icon), m_iconRect.size(), 15);
}

void KiranModeClassItemWgt::setTextVisible(const bool &visible)
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

void KiranModeClassItemWgt::setText(const QString &text)
{
    m_text = text;
}

int KiranModeClassItemWgt::textModeWd()
{
    return 2*cClassItemPadding + cClassItemSpace + cClassItemIconWd + cClassItemTextWd;
}

int KiranModeClassItemWgt::iconModeWd()
{
    return 2*cClassItemPadding + cClassItemIconWd;
}

int KiranModeClassItemWgt::heightInt()
{
    return 2*cClassItemPadding+cClassItemIconWd;
}

