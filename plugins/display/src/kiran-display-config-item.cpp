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
 * Author:     yangxiaoqing <yangxiaoqing@kylinsec.com.cn>
 */

#include "kiran-display-config-item.h"
#include <QMouseEvent>
#include <QPainter>

KiranDisplayConfigItem::KiranDisplayConfigItem(QWidget *parent) : QPushButton(parent), m_mousePress(false), m_mouseDrag(false),
    m_statusType(QEvent::None), m_enabled(true), m_anchorByBtn(NULL), m_rotateDrect(DISPLAY_ROTATION_0), m_displayReflectType(DISPLAY_REFLECT_NORMAL)
{
    setAccessibleName("KiranDisplayConfigItem");
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
    if(m_enabled)
        pen.setColor("#ffffff");
    else
        pen.setColor(QColor(255,255,255,60));

    QBrush brush;
    if(isChecked())
    {
        pen.setWidth(1);
        brush = QBrush("#2eb3ff");
        painter.setPen(pen);
        painter.fillRect(rect, brush);
        rect.adjust(0, 0, -1, -1);
        painter.drawRect(rect);
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
            brush = QBrush("#b3b3b3");
            if(m_enabled) pen.setColor("#000000");
            break;
        }

        painter.setPen(pen);
        painter.fillRect(rect, brush);
    }

    //矩形绘制完成后，再处理文字旋转、镜像问题。缩小旋转、镜像的影响范围。
    //先旋转
    int flag = 0;//以X轴翻转。
    int flag2 = 0; //以Y轴翻转。
    switch (m_rotateDrect) {
    case DISPLAY_ROTATION_0://含义逆时针
    {

        rect.moveTo(0, 0);
        flag = -1;
        flag2 = -1;
    }
        break;
    case DISPLAY_ROTATION_90://含义逆时针
    {
        painter.rotate(270);//顺时针旋转
        int height = rect.height();
        rect.setHeight(rect.width());
        rect.setWidth(height);
        rect.moveTo(-rect.width(), 0);
        flag = -1;
        flag2 = 1;
    }
        break;
    case DISPLAY_ROTATION_180:
    {
        painter.rotate(180);
        rect.moveTo(-rect.width(), -rect.height());
        flag = 1;
        flag2 = 1;
    }
        break;
    case DISPLAY_ROTATION_270:
    {
        painter.rotate(90);
        int height = rect.height();
        rect.setHeight(rect.width());
        rect.setWidth(height);
        rect.moveTo(0, -rect.height());
        flag = 1;
        flag2 = -1;
    }
        break;
    default:
        break;
    }

    //x,y轴翻转。
    qreal sx = 1, sy = 1;
    if(m_displayReflectType & DISPLAY_REFLECT_Y)
    {
        int textHeight = painter.fontMetrics().ascent() - painter.fontMetrics().descent();
        rect.moveTo(rect.x(), rect.y() + flag*rect.height() - textHeight/5);//防止文字翻转之后，位置变动。为什么是除以5
        sy = -1;
    }
    if(m_displayReflectType & DISPLAY_REFLECT_X)
    {
        rect.moveTo(rect.x() + flag2*rect.width(), rect.y());
        sx = -1;
    }
    painter.scale(sx, sy);

    painter.drawText(rect, Qt::TextWrapAnywhere|Qt::AlignCenter, text());
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

DisplayRotationType KiranDisplayConfigItem::rotateDrect() const
{
    return m_rotateDrect;
}

void KiranDisplayConfigItem::alterRotateDrect(const int &step)
{
    DisplayRotationType rotateDrect = rotationType(m_rotateDrect, step);
    switch (rotateDrect) {
    case DISPLAY_ROTATION_90:
    case DISPLAY_ROTATION_270:
    {
        if(m_rotateDrect == DISPLAY_ROTATION_0 || m_rotateDrect == DISPLAY_ROTATION_180)
        {
            QRectF t = m_screenGeometryF;
            m_screenGeometryF.setWidth(t.height());
            m_screenGeometryF.setHeight(t.width());
            emit sigDrag(this);
            emit sigEndDrag(this);
        }
    }
        break;
    case DISPLAY_ROTATION_0:
    case DISPLAY_ROTATION_180:
    {
        if(m_rotateDrect == DISPLAY_ROTATION_90 || m_rotateDrect == DISPLAY_ROTATION_270)
        {
            QRectF t = m_screenGeometryF;
            m_screenGeometryF.setWidth(t.height());
            m_screenGeometryF.setHeight(t.width());
            emit sigDrag(this);
            emit sigEndDrag(this);
        }
    }
        break;
    default:
        break;
    }

    m_rotateDrect = rotateDrect;
}

void KiranDisplayConfigItem::initRotateDrect(const DisplayRotationType &rotateDrect)
{
    switch (rotateDrect) {
    case DISPLAY_ROTATION_90:
    case DISPLAY_ROTATION_270:
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

DisplayReflectTypes KiranDisplayConfigItem::displayReflectType() const
{
    return m_displayReflectType;
}

void KiranDisplayConfigItem::setDisplayReflectType(const DisplayReflectTypes &displayReflectType)
{
    m_displayReflectType = displayReflectType;
    update();
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

DisplayRotationType KiranDisplayConfigItem::rotationType(const DisplayRotationType &curType, const int &step)
{
    QList<DisplayRotationType> list;
    list << DISPLAY_ROTATION_0 << DISPLAY_ROTATION_90 << DISPLAY_ROTATION_180 << DISPLAY_ROTATION_270;

    int index = list.indexOf(curType);
    index += step;
    index %= 4;
    if(index < 0) index += 4;

    if(index<0 || index >3) return DISPLAY_ROTATION_0;
    return list.at(index);
}

bool KiranDisplayConfigItem::enabled() const
{
    return m_enabled;
}
/*!
 * \brief KiranDisplayConfigItem::setEnabled setData函数中调用了此函数，如果在这里发送按钮禁用信号，外部再调用contain，可能此时item还未加入contain中，从而导致异常。
 * \param enabled
 */
void KiranDisplayConfigItem::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void KiranDisplayConfigItem::changeEnabled(const bool &enabled)
{
    m_enabled = enabled;
    update();
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
