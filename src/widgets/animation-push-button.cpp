//
// Created by lxh on 2020/11/4.
//

#include <QPropertyAnimation>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

#include "animation-push-button.h"
#include "animation-button-delegate.h"

AnimationPushButton::AnimationPushButton(QWidget *parent)
        : QPushButton(parent),
          m_animationDelegate(new AnimationButtonDelegate(this)) {
    initAnimation();
}

int AnimationPushButton::circleSize() {
    return m_circleSize;
}

int AnimationPushButton::circleWidth() {
    return m_circleWidth;
}

QColor AnimationPushButton::circleColor() {
    return m_circleColor;
}

void AnimationPushButton::setBusy(bool busy) {
    if (m_isBusy == busy) {
        return;
    }
    m_isBusy = busy;
    if (m_isBusy)
        m_animationGroup->start();
    else
        m_animationGroup->stop();
}

bool AnimationPushButton::busy() {
    return m_isBusy;
}

void AnimationPushButton::setCircleSize(int size) {
    if (m_circleSize == size)
        return;
    m_circleSize = size;
    update();
}

void AnimationPushButton::setCircleWidth(int width) {
    if (m_circleWidth == width)
        return;
    m_circleWidth = width;
    if (m_isBusy)
        update();
}

void AnimationPushButton::setCircleColor(QColor color) {
    if (m_circleColor == color)
        return;
    m_circleColor = color;
    if (m_isBusy)
        update();
}

void AnimationPushButton::initAnimation() {
    m_animationGroup = new QParallelAnimationGroup(this);
    m_animationGroup->setLoopCount(-1);


    auto propertyAnimation = new QPropertyAnimation(this);
    m_animationGroup->addAnimation(propertyAnimation);
    propertyAnimation->setTargetObject(m_animationDelegate);
    propertyAnimation->setPropertyName("dashLength");
    propertyAnimation->setStartValue(0.1);
    propertyAnimation->setKeyValueAt(0.15, 0.2);
    propertyAnimation->setKeyValueAt(0.6, 20);
    propertyAnimation->setKeyValueAt(0.7, 20);
    propertyAnimation->setEndValue(20);
    propertyAnimation->setDuration(2050);

    propertyAnimation = new QPropertyAnimation(this);
    m_animationGroup->addAnimation(propertyAnimation);
    propertyAnimation->setTargetObject(m_animationDelegate);
    propertyAnimation->setPropertyName("dashOffset");
    propertyAnimation->setStartValue(0);
    propertyAnimation->setKeyValueAt(0.15, 0);
    propertyAnimation->setKeyValueAt(0.6, -7);
    propertyAnimation->setKeyValueAt(0.7, -7);
    propertyAnimation->setEndValue(-25);
    propertyAnimation->setDuration(2050);

    propertyAnimation = new QPropertyAnimation(this);
    m_animationGroup->addAnimation(propertyAnimation);
    propertyAnimation->setTargetObject(m_animationDelegate);
    propertyAnimation->setPropertyName("angle");
    propertyAnimation->setStartValue(0);
    propertyAnimation->setEndValue(719);
    propertyAnimation->setDuration(2050);
}

void AnimationPushButton::paintEvent(QPaintEvent *event) {
    if (m_isBusy && isEnabled()) {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        //绘制背景
        QStyleOption opt;
        opt.init(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

        //绘制等待线条
        QPen pen;
        pen.setCapStyle(Qt::RoundCap);
        pen.setWidthF(m_circleWidth);
        pen.setColor(m_circleColor);
        painter.translate(width()/2, height()/2);
        painter.rotate(m_animationDelegate->angle());
        QVector<qreal> pattern;
        pattern << m_animationDelegate->dashLength()*m_circleSize/50 << 30*m_circleSize/50;
        pen.setDashOffset(m_animationDelegate->dashOffset()*m_circleSize/50);
        pen.setDashPattern(pattern);
        painter.setPen(pen);

        painter.drawEllipse(QPoint(0, 0), m_circleSize/2, m_circleSize/2);
    } else {
        QPushButton::paintEvent(event);
    }
}
