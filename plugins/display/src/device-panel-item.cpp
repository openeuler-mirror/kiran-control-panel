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

#include "device-panel-item.h"
#include <QMouseEvent>
#include <QPainter>

DevicePanelItem::DevicePanelItem(const QString &monitorPath, QWidget *parent)
    : QPushButton(parent),
      m_mousePress(false),
      m_mouseDrag(false),
      m_statusType(QEvent::None),
      m_anchorByBtn(NULL),
      m_enabled(true),
      m_rotateDrect(DISPLAY_ROTATION_0),
      m_displayReflectType(DISPLAY_REFLECT_NORMAL)
{
    setAccessibleName("DevicePanelItem");
    setCheckable(true);
    setAttribute(Qt::WA_Hover, true);
    installEventFilter(this);
    setStyleSheet("QToolTip{color:#000000;border: 0px solid #ffffff;background:#b3b3b3;}");
    setCursor(Qt::OpenHandCursor);

    m_monitorPath = monitorPath;
    m_monitorConfigData = DisplayConfig::instance()->getMonitorConfigData(m_monitorPath);
    init();

    connect(m_monitorConfigData.data(), &MonitorConfigData::resolvingChanged, this, &DevicePanelItem::handleConfigResolvingChanged);
}

void DevicePanelItem::init()
{
    setText(m_monitorConfigData->name());
    setToolTip(m_monitorConfigData->name());

    QPoint pos = m_monitorConfigData->position();
    QRectF rect(pos.x(), pos.y(), m_monitorConfigData->width(), m_monitorConfigData->height());
    m_screenGeometryF = rect;
    m_enabled = m_monitorConfigData->enabled();

    initRotateDrect(m_monitorConfigData->rotation());
    setDisplayReflectType(m_monitorConfigData->reflect());
}

void DevicePanelItem::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (m_mouseDrag) painter.setOpacity(0.7);
    QRect rect = this->rect();
    rect.adjust(1, 1, -1, -1);

    // QFontMetrics fm = painter.fontMetrics();
    // float zoom = rect.width() / fm.width(text());
    float pixsize = 22;  // zoom*15.0;
    QFont font;
    font.setPixelSize(pixsize);
    painter.setFont(font);

    QPen pen;
    if (m_enabled)
        pen.setColor("#ffffff");
    else
        pen.setColor(QColor(255, 255, 255, 60));

    QBrush brush;
    if (isChecked())
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
        switch (m_statusType)
        {
        case QEvent::HoverEnter:  // hover
            brush = QBrush(QColor(121, 195, 255));
            break;
        case QEvent::FocusIn:  // checked
            brush = QBrush("#2eb3ff");
            break;
        default:
            brush = QBrush("#b3b3b3");
            if (m_enabled) pen.setColor("#000000");
            break;
        }

        painter.setPen(pen);
        painter.fillRect(rect, brush);
    }

    // 矩形绘制完成后，再处理文字旋转、镜像问题。缩小旋转、镜像的影响范围。
    // 先旋转
    int flag = 0;   // 以X轴翻转。
    int flag2 = 0;  // 以Y轴翻转。
    switch (m_rotateDrect)
    {
    case DISPLAY_ROTATION_0:  // 含义逆时针
    {
        rect.moveTo(0, 0);
        flag = -1;
        flag2 = -1;
    }
    break;
    case DISPLAY_ROTATION_90:  // 含义逆时针
    {
        painter.rotate(270);   // 顺时针旋转
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

    // x,y轴翻转。
    qreal sx = 1, sy = 1;
    if (m_displayReflectType & DISPLAY_REFLECT_Y)
    {
        int textHeight = painter.fontMetrics().ascent() - painter.fontMetrics().descent();
        rect.moveTo(rect.x(), rect.y() + flag * rect.height() - textHeight / 5);  // 防止文字翻转之后，位置变动。为什么是除以5
        sy = -1;
    }
    if (m_displayReflectType & DISPLAY_REFLECT_X)
    {
        rect.moveTo(rect.x() + flag2 * rect.width(), rect.y());
        sx = -1;
    }
    painter.scale(sx, sy);

    painter.drawText(rect, Qt::TextWrapAnywhere | Qt::AlignCenter, text());
}

bool DevicePanelItem::eventFilter(QObject *obj, QEvent *event)
{
    if (Q_LIKELY(obj == this))
    {
        switch (event->type())
        {
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

void DevicePanelItem::alterRotateDrect(const int &step)
{
    DisplayRotationType rotateDrect = rotationType(m_rotateDrect, step);
    switch (rotateDrect)
    {
    case DISPLAY_ROTATION_90:
    case DISPLAY_ROTATION_270:
    {
        if (m_rotateDrect == DISPLAY_ROTATION_0 || m_rotateDrect == DISPLAY_ROTATION_180)
        {
            QRectF t = m_screenGeometryF;
            m_screenGeometryF.setWidth(t.height());
            m_screenGeometryF.setHeight(t.width());
            emit drag(this);
            emit endDrag(this);
        }
    }
    break;
    case DISPLAY_ROTATION_0:
    case DISPLAY_ROTATION_180:
    {
        if (m_rotateDrect == DISPLAY_ROTATION_90 || m_rotateDrect == DISPLAY_ROTATION_270)
        {
            QRectF t = m_screenGeometryF;
            m_screenGeometryF.setWidth(t.height());
            m_screenGeometryF.setHeight(t.width());
            emit drag(this);
            emit endDrag(this);
        }
    }
    break;
    default:
        break;
    }

    m_rotateDrect = rotateDrect;
    m_monitorConfigData->setRotation(rotateDrect);
}

void DevicePanelItem::initRotateDrect(const DisplayRotationType &rotateDrect)
{
    switch (rotateDrect)
    {
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

void DevicePanelItem::setDisplayReflectType(const DisplayReflectTypes &displayReflectType)
{
    m_displayReflectType = displayReflectType;
    m_monitorConfigData->setReflect(displayReflectType);
    update();
}

QPair<int, int> DevicePanelItem::zoomPair() const
{
    return m_zoomPair;
}

void DevicePanelItem::setZoomPair(const QPair<int, int> &zoomPair)
{
    m_zoomPair = zoomPair;
}

void DevicePanelItem::removeAnchoredChildBtn(DevicePanelItem *childBtn)
{
    m_childAnchorBtns.removeAll(childBtn);
}

void DevicePanelItem::appendAnchoredChildBtn(DevicePanelItem *childBtn)
{
    m_childAnchorBtns.append(childBtn);
}

void DevicePanelItem::clearAnchorByBtn()
{
    m_anchorByBtn = NULL;
}

void DevicePanelItem::clearAnchoredChildBtns()
{
    foreach (DevicePanelItem *item, m_childAnchorBtns)
    {
        item->clearAnchorByBtn();
    }
    m_childAnchorBtns.clear();
}

bool DevicePanelItem::hasIntersects(DevicePanelItem *item)
{
    QRectF geometry = item->screenGeometryF();
    if (this->screenGeometryF().intersects(geometry)) return true;

    return false;
}

DevicePanelItem::AnchorByDrect DevicePanelItem::anchorByDrect() const
{
    return m_anchorByDrect;
}

void DevicePanelItem::setAnchorByDrect(const AnchorByDrect &anchorByDrect)
{
    m_anchorByDrect = anchorByDrect;
}

DevicePanelItem *DevicePanelItem::anchorByBtn() const
{
    return m_anchorByBtn;
}

void DevicePanelItem::setAnchorByBtn(DevicePanelItem *anchorByBtn, const DevicePanelItem::AnchorByDrect &anchorByDrect)
{
    //
    if (m_anchorByBtn)
    {
        m_anchorByBtn->removeAnchoredChildBtn(this);
    }
    // now
    m_anchorByBtn = anchorByBtn;
    m_anchorByDrect = anchorByDrect;
    if (anchorByBtn) anchorByBtn->appendAnchoredChildBtn(this);
    // update pos
    updateOffset(anchorByBtn, anchorByDrect, m_mouseDrag);
    updateScreenGeometry();
    //
    clearAnchoredChildBtns();
}

void DevicePanelItem::updateScreenGeometry()
{
    if (!m_anchorByBtn) return;
    const QRectF &anchorScreenGeometry = m_anchorByBtn->screenGeometryF();
    switch (m_anchorByDrect)
    {
    case DevicePanelItem::PosLeft:
        m_screenGeometryF.moveRight(anchorScreenGeometry.left());
        m_screenGeometryF.moveTop(anchorScreenGeometry.top() + m_screenOffset.y());
        break;
    case DevicePanelItem::PosTop:
        m_screenGeometryF.moveBottom(anchorScreenGeometry.top());
        m_screenGeometryF.moveLeft(anchorScreenGeometry.left() + m_screenOffset.x());
        break;
    case DevicePanelItem::PosRight:
        m_screenGeometryF.moveLeft(anchorScreenGeometry.right());
        m_screenGeometryF.moveTop(anchorScreenGeometry.top() + m_screenOffset.y());
        break;
    case DevicePanelItem::PosBottom:
        m_screenGeometryF.moveTop(anchorScreenGeometry.bottom());
        m_screenGeometryF.moveLeft(anchorScreenGeometry.left() + m_screenOffset.x());
        break;
    case DevicePanelItem::PosTopLeft:
        m_screenGeometryF.moveBottomRight(anchorScreenGeometry.topLeft());
        break;
    case DevicePanelItem::PosTopRight:
        m_screenGeometryF.moveBottomLeft(anchorScreenGeometry.topRight());
        break;
    case DevicePanelItem::PosBottomLeft:
        m_screenGeometryF.moveTopRight(anchorScreenGeometry.bottomLeft());
        break;
    case DevicePanelItem::PosBottomRight:
        m_screenGeometryF.moveTopLeft(anchorScreenGeometry.bottomRight());
    default:
        break;
    }
    m_monitorConfigData->setPosition(m_screenGeometryF.x(), m_screenGeometryF.y());
}

void DevicePanelItem::updateOffset(DevicePanelItem *anchorByBtn, const DevicePanelItem::AnchorByDrect &anchorByDrect, const bool &isDrag)
{
    if (!anchorByBtn) return;

    const QRectF &screenGeometry = this->screenGeometryF();
    const QRectF &anchorScreenGeometry = anchorByBtn->screenGeometryF();
    switch (anchorByDrect)
    {
    case DevicePanelItem::PosLeft:
    case DevicePanelItem::PosRight:
        if (isDrag)
            m_screenOffset = QPointF(0, anchorScreenGeometry.height() * m_zoomPair.first / m_zoomPair.second);
        else
            m_screenOffset = QPointF(0, screenGeometry.top() - anchorScreenGeometry.top());
        break;
    case DevicePanelItem::PosTop:
    case DevicePanelItem::PosBottom:
        if (isDrag)
            m_screenOffset = QPointF(anchorScreenGeometry.width() * m_zoomPair.first / m_zoomPair.second, 0);
        else
            m_screenOffset = QPointF(screenGeometry.left() - anchorScreenGeometry.left(), 0);
        break;
    case DevicePanelItem::PosTopLeft:
    case DevicePanelItem::PosTopRight:
    case DevicePanelItem::PosBottomLeft:
    case DevicePanelItem::PosBottomRight:
        m_screenOffset = QPointF(0, 0);
    default:
        break;
    }
}

DisplayRotationType DevicePanelItem::rotationType(const DisplayRotationType &curType, const int &step)
{
    QList<DisplayRotationType> list;
    list << DISPLAY_ROTATION_0 << DISPLAY_ROTATION_90 << DISPLAY_ROTATION_180 << DISPLAY_ROTATION_270;

    int index = list.indexOf(curType);
    index += step;
    index %= 4;
    if (index < 0) index += 4;

    if (index < 0 || index > 3) return DISPLAY_ROTATION_0;
    return list.at(index);
}

bool DevicePanelItem::enabled() const
{
    return m_enabled;
}

void DevicePanelItem::handleConfigResolvingChanged(const QSize &size)
{
    m_screenGeometryF.setWidth(m_monitorConfigData->width());
    m_screenGeometryF.setHeight(m_monitorConfigData->height());

    emit screenGeometryChanged();
}

/*!
 * \brief DevicePanelItem::setEnabled setData函数中调用了此函数，如果在这里发送按钮禁用信号，外部再调用contain，可能此时item还未加入contain中，从而导致异常。
 * \param enabled
 */
void DevicePanelItem::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void DevicePanelItem::changeEnabled(const bool &enabled)
{
    m_enabled = enabled;
    update();
}

QString DevicePanelItem::monitorPath() const
{
    return m_monitorPath;
}

QRectF DevicePanelItem::screenGeometryF() const
{
    return m_screenGeometryF;
}

void DevicePanelItem::setScreenGeometryF(const QRectF &screenGeometryF)
{
    m_screenGeometryF = screenGeometryF;
}

void DevicePanelItem::moveScreenGeometryFOffset(const QPointF &offsetF)
{
    m_screenGeometryF.moveTopLeft(m_screenGeometryF.topLeft() + offsetF);
    m_monitorConfigData->setPosition(m_screenGeometryF.x(), m_screenGeometryF.y());
}

void DevicePanelItem::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        raise();
        m_mousePress = true;
        m_pressPos = e->localPos().toPoint();
    }

    QPushButton::mousePressEvent(e);
}

void DevicePanelItem::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePress)
    {
        m_mouseDrag = true;
        move(mapTo(parentWidget(), e->pos()) - m_pressPos);
        emit drag(this);
    }

    QPushButton::mouseMoveEvent(e);
}

void DevicePanelItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_mousePress = false;
        if (m_mouseDrag)
        {
            emit endDrag(this);
            m_mouseDrag = false;
        }
    }

    QPushButton::mouseReleaseEvent(e);
}
