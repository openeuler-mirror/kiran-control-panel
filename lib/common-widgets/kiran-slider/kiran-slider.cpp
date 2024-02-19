/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#include "kiran-slider.h"
#include <style-palette.h>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

using namespace Kiran;

static const int sliderMargin = 16;

static const int sliderGrooveHeight = 4;

static const int sliderHandleHeight = 16;
static const int sliderHandleWidth = 12;

static const int tickSpace = 2;

static const int ticknessHeight = 12;
static const int ticknessWedith = 2;

KiranSlider::MarkPoint::MarkPoint(int value_, QString desc_)
    : value(value_),
      desc(desc_)
{
}

bool KiranSlider::UpdateContext::operator==(const UpdateContext& other) const
{
    if (size != other.size ||
        max != other.max ||
        min != other.min ||
        value != other.value)
    {
        return false;
    }

    return true;
}

KiranSlider::UpdateContext KiranSlider::getCurrentUpdateContext()
{
    KiranSlider::UpdateContext updateCache;
    updateCache.min = minimum();
    updateCache.max = maximum();
    updateCache.size = size();
    updateCache.value = value();
    return updateCache;
}

KiranSlider::KiranSlider(QWidget* parent)
    : QAbstractSlider(parent)
{
    connect(this, &QAbstractSlider::valueChanged, this, &KiranSlider::ensureLayoutUpdated);
    connect(this, &QAbstractSlider::rangeChanged, this, &KiranSlider::ensureLayoutUpdated);
}

KiranSlider::~KiranSlider()
{
}

QSize KiranSlider::sizeHint() const
{
    const int sliderLength = 160;

    int w = 2 * sliderMargin, h = 2 * sliderMargin;
    w += sliderLength;

    h += qMax(sliderGrooveHeight, sliderHandleHeight);
    h += 2 * tickSpace;
    h += fontMetrics().height();

    return QSize(w, h);
}

QSize KiranSlider::minimumSizeHint() const
{
    const int sliderLength = 160;

    int w = 2 * sliderMargin, h = 2 * sliderMargin;
    w += sliderLength;

    h += qMax(sliderGrooveHeight, sliderHandleHeight);
    h += 2 * tickSpace;
    h += fontMetrics().height();

    return QSize(w, h);
}

void KiranSlider::addMarks(QList<MarkPoint> marks)
{
    for (auto markPoint : marks)
    {
        addMark(markPoint, false);
    }
    update();
}

void KiranSlider::addMark(MarkPoint mark, bool needUpdateUI)
{
    bool inserted = false;

    for (auto markItem = m_markPoints.begin();
         markItem != m_markPoints.end();
         markItem++)
    {
        if (mark.value < markItem->value)
        {
            m_markPoints.insert(markItem, mark);
            inserted = true;
            break;
        }
    }

    if (!inserted)
    {
        m_markPoints.append(mark);
    }

    if (needUpdateUI)
    {
        update();
    }
}

void KiranSlider::deleteMark(int markValue)
{
    for (auto markItem = m_markPoints.begin();
         markItem != m_markPoints.end();)
    {
        if (markItem->value == markValue)
        {
            m_markPoints.erase(markItem);
            continue;
        }
        markItem++;
    }

    update();
}

void KiranSlider::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    drawBackground(painter);
    drawSliderGroove(painter);
    drawSliderHandle(painter);
    drawTickmarks(painter);
}

void KiranSlider::resizeEvent(QResizeEvent* event)
{
    ensureLayoutUpdated();
    QAbstractSlider::resizeEvent(event);
}

void KiranSlider::mousePressEvent(QMouseEvent* ev)
{
    if (maximum() == minimum() || (ev->buttons() ^ ev->button()))
    {
        ev->ignore();
        return;
    }

    ev->accept();

    // 扩大滑动槽触发区域
    auto grooveTriggerRect = m_grooveRect.adjusted(-1, -1, 1, 1);

    if (m_handleRect.contains(ev->pos()))
    {
        // 按压滑块
        m_pressedHandle = true;
        setSliderDown(true);
    }
    else if (grooveTriggerRect.contains(ev->pos()))
    {
        // 按压滑块槽
        int offset = ev->pos().x() - grooveTriggerRect.left();
        int value = sliderValueFromPosition(minimum(), maximum(), offset, grooveTriggerRect.width(), false);
        setSliderPosition(value);
    }

    return;
}

void KiranSlider::mouseReleaseEvent(QMouseEvent* ev)
{
    if (ev->buttons() || !m_pressedHandle)
    {
        ev->ignore();
        return;
    }

    setRepeatAction(SliderNoAction);
    setSliderDown(false);
}

void KiranSlider::mouseMoveEvent(QMouseEvent* ev)
{
    if (!m_pressedHandle)
    {
        ev->ignore();
        return;
    }

    ev->accept();

    // 扩大滑动槽触发区域
    auto grooveTriggerRect = m_grooveRect.adjusted(-1, -1, 1, 1);

    int offset = ev->pos().x() - grooveTriggerRect.left();
    int value = sliderValueFromPosition(minimum(), maximum(), offset, grooveTriggerRect.width(), false);

    setSliderPosition(value);
}

void KiranSlider::ensureLayoutUpdated()
{
    // 计算是否需要更新布局
    auto currentContext = getCurrentUpdateContext();
    if (currentContext == m_updateCache)
    {
        return;
    }

    // 更新布局
    auto groove = QRect(sliderMargin,
                        sliderMargin + (sliderHandleHeight - sliderGrooveHeight) / 2,
                        width() - (sliderMargin * 2),
                        sliderGrooveHeight);

    int offset = sliderPositionFromValue(minimum(), maximum(), value(), groove.width(), false);
    auto content = groove;
    content.setRight(groove.x() + offset);

    auto handle = QRect(0, 0, sliderHandleWidth, sliderHandleHeight);
    handle.moveCenter(QPoint(content.right(), content.center().y()));

    m_grooveRect = groove;
    m_contentRect = content;
    m_handleRect = handle;

    // 缓存当前上下文,避免频繁更新
    m_updateCache = currentContext;
}

void KiranSlider::drawBackground(QPainter& painter)
{
    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(rect(), 6, 6);

    painter.save();

    auto backgroundColor = StylePalette::instance()->color(StylePalette::Normal,
                                                           StylePalette::Widget,
                                                           StylePalette::Background);
    painter.fillPath(backgroundPath, backgroundColor);

    painter.restore();
}

void KiranSlider::drawSliderGroove(QPainter& painter)
{
    auto backgroundColor = StylePalette::instance()->color(StylePalette::Normal,
                                                           StylePalette::Bare,
                                                           StylePalette::Background);

    auto foregroundColor = StylePalette::instance()->color(StylePalette::Checked,
                                                           StylePalette::Bare,
                                                           StylePalette::Foreground);

    painter.save();
    painter.fillRect(m_grooveRect, backgroundColor);
    painter.fillRect(m_contentRect, foregroundColor);
    painter.restore();
}

void KiranSlider::drawTickmarks(QPainter& painter)
{
    for (auto mark : m_markPoints)
    {
        if (mark.value < minimum())
        {
            continue;
        }

        if (mark.value > maximum())
        {
            return;
        }

        drawTickmark(painter, mark);
    }
}

void KiranSlider::drawSliderHandle(QPainter& painter)
{
    auto foregroundColor = StylePalette::instance()->color(StylePalette::Checked,
                                                           StylePalette::Bare,
                                                           StylePalette::Foreground);

    QRect cricleRect(0, 0, sliderHandleWidth, sliderHandleWidth);
    cricleRect.moveCenter(m_handleRect.center());
    cricleRect.moveTop(m_handleRect.top());

    QPainterPath criclePath;
    criclePath.addEllipse(cricleRect);

    QPainterPath trianglePath;
    QPointF point1(cricleRect.left(), cricleRect.bottom() - 3);
    QPointF point2(cricleRect.right() + 1, cricleRect.bottom() - 3);
    QPointF point3(point1.x() + (point2.x() - point1.x()) / 2, m_handleRect.bottom());
    trianglePath.addPolygon(QPolygonF({point1, point2, point3}));

    painter.save();
    painter.fillPath(criclePath, foregroundColor);
    painter.fillPath(trianglePath, foregroundColor);
    painter.restore();
}

void KiranSlider::drawTickmark(QPainter& painter,
                               const MarkPoint& point)
{
    QFont font("Noto Sans CJK SC regular", 12);
    QFontMetrics metrics(font);

    auto backgroundColor = StylePalette::instance()->color(StylePalette::Normal,
                                                           StylePalette::Bare,
                                                           StylePalette::Background);

    QPen pen = painter.pen();
    pen.setColor(backgroundColor);

    int offset = sliderPositionFromValue(minimum(),
                                         maximum(),
                                         point.value,
                                         m_grooveRect.width(),
                                         false);

    auto ticknessRect = QRect(0, 0, ticknessWedith, ticknessHeight);

    // 计算tickmark位置
    // 标尺移动滑块下tickSpace的位置
    ticknessRect.moveTop(m_grooveRect.bottom() + (sliderHandleHeight - sliderGrooveHeight) / 2 + tickSpace);
    // 标尺移动到指定标记位置下
    ticknessRect.moveLeft(m_grooveRect.left() + offset + ticknessRect.width() / 2);

    // 计算tickmark描述位置
    QRect fontRect(0, 0, metrics.width(point.desc), metrics.height());
    fontRect.moveCenter(ticknessRect.center());
    fontRect.moveTop(ticknessRect.bottom() + tickSpace);

    painter.save();
    painter.setPen(pen);
    painter.setFont(font);
    painter.fillRect(ticknessRect, backgroundColor);
    painter.drawText(fontRect, point.desc);

    painter.restore();
}

int KiranSlider::sliderPositionFromValue(int min, int max,
                                         int logicalValue, int span,
                                         bool upsideDown)
{
    if (span <= 0 || logicalValue < min || max <= min)
        return 0;
    if (logicalValue > max)
        return upsideDown ? span : min;

    uint range = max - min;
    uint p = upsideDown ? max - logicalValue : logicalValue - min;

    if (range > (uint)INT_MAX / 4096)
    {
        double dpos = (double(p)) / (double(range) / span);
        return int(dpos);
    }
    else if (range > (uint)span)
    {
        return (2 * p * span + range) / (2 * range);
    }
    else
    {
        uint div = span / range;
        uint mod = span % range;
        return p * div + (2 * p * mod + range) / (2 * range);
    }
    // equiv. to (p * span) / range + 0.5
    // no overflow because of this implicit assumption:
    // span <= 4096
}

int KiranSlider::sliderValueFromPosition(int min, int max,
                                         int pos, int span,
                                         bool upsideDown)
{
    if (span <= 0 || pos <= 0)
        return upsideDown ? max : min;
    if (pos >= span)
        return upsideDown ? min : max;

    uint range = max - min;

    if ((uint)span > range)
    {
        int tmp = (2 * pos * range + span) / (2 * span);
        return upsideDown ? max - tmp : tmp + min;
    }
    else
    {
        uint div = range / span;
        uint mod = range % span;
        int tmp = pos * div + (2 * pos * mod + span) / (2 * span);
        return upsideDown ? max - tmp : tmp + min;
    }
    // equiv. to min + (pos*range)/span + 0.5
    // no overflow because of this implicit assumption:
    // pos <= span < sqrt(INT_MAX+0.0625)+0.25 ~ sqrt(INT_MAX)
}