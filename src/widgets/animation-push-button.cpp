//
// Created by lxh on 2020/11/4.
//

#include <QDebug>
#include <QPainter>
#include <QPropertyAnimation>
#include <QStyleOption>

#include "animation-push-button.h"
#include "log.h"

AnimationPushButton::AnimationPushButton(QWidget *parent)
    : QPushButton(parent),
      m_svgRender(QString(":/images/loading.svg"), this)
{
    initTimeLine();
}

void AnimationPushButton::setBusy(bool busy)
{
    if (m_isBusy == busy)
        return;

    if (busy && !m_svgRender.isValid())
    {
        LOG_WARNING_S() << "AnimationPushButton: animation pixmap isNull!";
        return;
    }
    m_isBusy = busy;
    if (m_isBusy)
    {
        m_rotationAngle = 0;
        m_timeLine.setCurrentTime(0);
        m_timeLine.start();
    }
    else
    {
        m_timeLine.stop();
        m_timeLine.setCurrentTime(0);
        m_rotationAngle = 0;
    }
}

bool AnimationPushButton::busy()
{
    return m_isBusy;
}

void AnimationPushButton::paintEvent(QPaintEvent *event)
{
    if (m_isBusy && isEnabled())
    {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

        QStyleOption opt;
        opt.init(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

        if (m_svgRender.isValid())
        {
            painter.translate(this->rect().center());
            painter.rotate(m_rotationAngle);
            int svgDrawSize = qMin(width(), height()) - 40;
            QRect renderRect((width() - svgDrawSize) / 2 - width() / 2,
                             (height() - svgDrawSize) / 2 - height() / 2,
                             svgDrawSize,
                             svgDrawSize);
            m_svgRender.render(&painter, renderRect);
        }
    }
    else
    {
        QPushButton::paintEvent(event);
    }
}

void AnimationPushButton::initTimeLine()
{
    m_timeLine.setCurrentTime(0);
    m_timeLine.setLoopCount(0);
    m_timeLine.setUpdateInterval(50);
    m_timeLine.setDuration(750);
    m_timeLine.setFrameRange(0, 360);
    m_timeLine.setCurveShape(QTimeLine::LinearCurve);
    connect(&m_timeLine, &QTimeLine::frameChanged, [this](int value) {
        m_rotationAngle = value;
        update();
    });
}
