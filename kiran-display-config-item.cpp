#include "kiran-display-config-item.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

KiranDisplayConfigItem::KiranDisplayConfigItem(QWidget *parent) : QPushButton(parent), m_mousePress(false), m_mouseDrag(false), m_statusType(QEvent::None), m_anchorByBtn(NULL), m_rotateDrect(ROTATION_0)
{
    setCheckable(true);
    setAttribute(Qt::WA_Hover,true);
    installEventFilter(this);
    setStyleSheet("QToolTip{color:#000000;border: 0px solid #ffffff;background:#b3b3b3;}");
    setCursor(Qt::OpenHandCursor);
}

void KiranDisplayConfigItem::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(m_mouseDrag) painter.setOpacity(0.7);
    QRect rect = this->rect();
    rect.adjust(1, 1, -1, -1);

    //QFontMetrics fm = painter.fontMetrics();
    //float zoom = rect.width() / fm.width(text());
    float pixsize = 22;//zoom*15.0;
    pixsize = pixsize > 10.0 ? pixsize : 10.0;
    pixsize = pixsize < 60.0 ? pixsize : 60.0;
    QFont font;
    font.setPixelSize(pixsize);
    painter.setFont(font);

    QPen pen;
    QBrush brush;
    if(isChecked())
    {
        pen.setWidth(1);
        pen.setColor("#ffffff");
        brush = QBrush("#2eb3ff");
        painter.setPen(pen);
        painter.fillRect(rect, brush);
        painter.drawText(rect, Qt::TextWrapAnywhere|Qt::AlignCenter, text());
        rect.adjust(0, 0, -1, -1);
        painter.drawRect(rect);
    }
    else
    {
        switch (m_statusType) {
        case QEvent::HoverEnter://hover
            pen.setColor("#ffffff");
            brush = QBrush(QColor(121, 195, 255));
            break;
        case QEvent::FocusIn://checked
            pen.setColor("#ffffff");
            brush = QBrush("#2eb3ff");
            break;
        default:
            brush = QBrush("#b3b3b3");
            break;
        }

        painter.setPen(pen);
        painter.fillRect(rect, brush);
        painter.drawText(rect, Qt::TextWrapAnywhere|Qt::AlignCenter, text());
    }
}

bool KiranDisplayConfigItem::eventFilter(QObject * obj, QEvent * event)
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

    return QPushButton::eventFilter(obj, event);
}

void KiranDisplayConfigItem::initRotateDrect(const RotateDrect &rotateDrect)
{
    switch (rotateDrect) {
    case Left:
    case Right:
    {
        QRectF t = m_screenGeometryF;
        m_screenGeometryF.setWidth(t.height());
        m_screenGeometryF.setHeight(t.width());
    }
        break;
    default:
        break;
    }

    m_rotateDrect = rotateDrect;
}

KiranDisplayConfigItem::RotateDrect KiranDisplayConfigItem::rotateDrect() const
{
    return m_rotateDrect;
}

void KiranDisplayConfigItem::alterRotateDrect(const RotateDrect &rotateDrect)
{
    switch (rotateDrect) {
    case Left:
    case Right:
    {
        if(m_rotateDrect == ROTATION_0 || m_rotateDrect == Inverted)
        {
            QRectF t = m_screenGeometryF;
            m_screenGeometryF.setWidth(t.height());
            m_screenGeometryF.setHeight(t.width());
            emit sigDrag(this);
            emit sigEndDrag(this);
        }

        m_rotateDrect = rotateDrect;
    }
        break;
    case ROTATION_0:
    case Inverted:
    {
        if(m_rotateDrect == Left || m_rotateDrect == Right)
        {
            QRectF t = m_screenGeometryF;
            m_screenGeometryF.setWidth(t.height());
            m_screenGeometryF.setHeight(t.width());
            emit sigDrag(this);
            emit sigEndDrag(this);
        }

        m_rotateDrect = m_rotateDrect==Inverted ? ROTATION_0 : Inverted;
    }
        break;
    default:
        break;
    }
}

QPair<int, int> KiranDisplayConfigItem::zoomPair() const
{
    return m_zoomPair;
}

void KiranDisplayConfigItem::setZoomPair(const QPair<int, int> &zoomPair)
{
    m_zoomPair = zoomPair;
}

void KiranDisplayConfigItem::removeAnchoredChildBtn(KiranDisplayConfigItem *childBtn)
{
    m_childAnchorBtns.removeAll(childBtn);
}

void KiranDisplayConfigItem::appendAnchoredChildBtn(KiranDisplayConfigItem *childBtn)
{
    m_childAnchorBtns.append(childBtn);
}

void KiranDisplayConfigItem::clearAnchorByBtn()
{
    m_anchorByBtn = NULL;
}

void KiranDisplayConfigItem::clearAnchoredChildBtns()
{
    foreach (KiranDisplayConfigItem *item, m_childAnchorBtns) {
        item->clearAnchorByBtn();
    }
    m_childAnchorBtns.clear();
}

bool KiranDisplayConfigItem::hasIntersects(KiranDisplayConfigItem *item)
{
    QRectF geometry = item->screenGeometryF();
    if(this->screenGeometryF().intersects(geometry)) return true;

    return false;
}

KiranDisplayConfigItem::AnchorByDrect KiranDisplayConfigItem::anchorByDrect() const
{
    return m_anchorByDrect;
}

void KiranDisplayConfigItem::setAnchorByDrect(const AnchorByDrect &anchorByDrect)
{
    m_anchorByDrect = anchorByDrect;
}

KiranDisplayConfigItem *KiranDisplayConfigItem::anchorByBtn() const
{
    return m_anchorByBtn;
}

void KiranDisplayConfigItem::setAnchorByBtn(KiranDisplayConfigItem *anchorByBtn, const KiranDisplayConfigItem::AnchorByDrect &anchorByDrect)
{
    //
    if(m_anchorByBtn)
    {
        m_anchorByBtn->removeAnchoredChildBtn(this);
    }
    //now
    m_anchorByBtn = anchorByBtn;
    m_anchorByDrect = anchorByDrect;
    if(anchorByBtn) anchorByBtn->appendAnchoredChildBtn(this);
    //update pos
    updateOffset(anchorByBtn, anchorByDrect, m_mouseDrag);
    updateScreenGeometry();
    //
    clearAnchoredChildBtns();
}

void KiranDisplayConfigItem::updateScreenGeometry()
{
    if(!m_anchorByBtn) return;
    const QRectF &anchorScreenGeometry = m_anchorByBtn->screenGeometryF();
    switch (m_anchorByDrect) {
    case KiranDisplayConfigItem::PosLeft:
        m_screenGeometryF.moveRight(anchorScreenGeometry.left());
        m_screenGeometryF.moveTop(anchorScreenGeometry.top() + m_screenOffset.y());
        break;
    case KiranDisplayConfigItem::PosTop:
        m_screenGeometryF.moveBottom(anchorScreenGeometry.top());
        m_screenGeometryF.moveLeft(anchorScreenGeometry.left() + m_screenOffset.x());
        break;
    case KiranDisplayConfigItem::PosRight:
        m_screenGeometryF.moveLeft(anchorScreenGeometry.right());
        m_screenGeometryF.moveTop(anchorScreenGeometry.top() + m_screenOffset.y());
        break;
    case KiranDisplayConfigItem::PosBottom:
        m_screenGeometryF.moveTop(anchorScreenGeometry.bottom());
        m_screenGeometryF.moveLeft(anchorScreenGeometry.left() + m_screenOffset.x());
        break;
    case KiranDisplayConfigItem::PosTopLeft:
        m_screenGeometryF.moveBottomRight(anchorScreenGeometry.topLeft());
        break;
    case KiranDisplayConfigItem::PosTopRight:
        m_screenGeometryF.moveBottomLeft(anchorScreenGeometry.topRight());
        break;
    case KiranDisplayConfigItem::PosBottomLeft:
        m_screenGeometryF.moveTopRight(anchorScreenGeometry.bottomLeft());
        break;
    case KiranDisplayConfigItem::PosBottomRight:
        m_screenGeometryF.moveTopLeft(anchorScreenGeometry.bottomRight());
    default:
        break;
    }
}

void KiranDisplayConfigItem::updateOffset(KiranDisplayConfigItem *anchorByBtn, const KiranDisplayConfigItem::AnchorByDrect &anchorByDrect, const bool &isDrag)
{
    if(!anchorByBtn) return;

    const QRectF &screenGeometryF = this->screenGeometryF();
    const QRectF &anchorScreenGeometry = anchorByBtn->screenGeometryF();
    switch (anchorByDrect) {
    case KiranDisplayConfigItem::PosLeft:
    case KiranDisplayConfigItem::PosRight:
        if(isDrag)
            m_screenOffset = QPointF(0, anchorScreenGeometry.height() * m_zoomPair.first / m_zoomPair.second);
        else
            m_screenOffset = QPointF(0, screenGeometryF.top() - anchorScreenGeometry.top());
        break;
    case KiranDisplayConfigItem::PosTop:
    case KiranDisplayConfigItem::PosBottom:
        if(isDrag)
            m_screenOffset = QPointF(anchorScreenGeometry.width() * m_zoomPair.first / m_zoomPair.second, 0);
        else
            m_screenOffset = QPointF(screenGeometryF.left() - anchorScreenGeometry.left(), 0);
        break;
    case KiranDisplayConfigItem::PosTopLeft:
    case KiranDisplayConfigItem::PosTopRight:
    case KiranDisplayConfigItem::PosBottomLeft:
    case KiranDisplayConfigItem::PosBottomRight:
        m_screenOffset = QPointF(0, 0);
    default:
        break;
    }
}

QString KiranDisplayConfigItem::monitorPath() const
{
    return m_monitorPath;
}

void KiranDisplayConfigItem::setMonitorPath(const QString &monitorPath)
{
    m_monitorPath = monitorPath;
}

QRectF KiranDisplayConfigItem::screenGeometryF() const
{
    return m_screenGeometryF;
}

QRectF &KiranDisplayConfigItem::screenGeometryF2()
{
    return m_screenGeometryF;
}

void KiranDisplayConfigItem::setScreenGeometryF(const QRectF &screenGeometryF)
{
    m_screenGeometryF = screenGeometryF;
}

void KiranDisplayConfigItem::moveScreenGeometryFOffset(const QPointF &offsetF)
{
    m_screenGeometryF.moveTopLeft(m_screenGeometryF.topLeft() + offsetF);
}

void KiranDisplayConfigItem::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        raise();
        m_mousePress = true;
        m_pressPos = e->localPos().toPoint();
    }

    QPushButton::mousePressEvent(e);
}

void KiranDisplayConfigItem::mouseMoveEvent(QMouseEvent *e)
{
    if(m_mousePress)
    {
        m_mouseDrag = true;
        move(mapTo(parentWidget(), e->pos())- m_pressPos);
        emit sigDrag(this);
    }

    QPushButton::mouseMoveEvent(e);
}

void KiranDisplayConfigItem::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_mousePress = false;
        if(m_mouseDrag)
        {
            emit sigEndDrag(this);
            m_mouseDrag = false;
        }
    }

    QPushButton::mouseReleaseEvent(e);
}
